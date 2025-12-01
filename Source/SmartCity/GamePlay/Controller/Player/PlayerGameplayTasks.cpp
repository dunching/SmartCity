#include "PlayerGameplayTasks.h"

#include "DatasmithAssetUserData.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Tasks/AITask.h"
#include "Engine/Light.h"

#include "GameOptions.h"
#include "SceneInteractionWorldSystem.h"
#include "Algorithm.h"
#include "AssetRefMap.h"
#include "TourPawn.h"
#include "Tools.h"
#include "GameplayTagsLibrary.h"
#include "AssetRefMap.h"
#include "Building_CurtainWall.h"
#include "TowerHelperBase.h"
#include "DatasmithSceneActor.h"
#include "FloorHelper.h"
#include "GenerateTypes.h"
#include "LogWriter.h"
#include "ReplaceActor.h"
#include "SceneElementBase.h"
#include "TemplateHelper.h"
#include "CollisionDataStruct.h"
#include "Elevator.h"
#include "LandScapeBase.h"
#include "SceneElementCategory.h"
#include "SceneElement_ControlBorder.h"
#include "SceneElement_LandScape.h"
#include "SceneElement_PWR_Pipe.h"
#include "SceneElement_Regualar.h"
#include "SceneElement_Space.h"
#include "SmartCitySuiteTags.h"
#include "ViewerPawnBase.h"

struct FPrefix : public TStructVariable<FPrefix>
{
	FName Datasmith_UniqueId = TEXT("Datasmith_UniqueId");
};

FName UPlayerControllerGameplayTasksComponent::ComponentName = TEXT("PlayerControllerGameplayTasksComponent");

void UGT_ReplyCameraTransform::Activate()
{
	Super::Activate();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(
	                                      this,
	                                      AViewerPawnBase::StaticClass(),
	                                      OutActors
	                                     );

	for (auto ActorIter : OutActors)
	{
		auto ViewerPawnPtr = Cast<AViewerPawnBase>(ActorIter);
		if (ViewerPawnPtr)
		{
			if (ViewerPawnPtr->SeatTag == SeatTag)
			{
				TargetLocation = ViewerPawnPtr->GetActorLocation();
				TargetRotation = ViewerPawnPtr->GetActorRotation();
				TargetTargetArmLength = ViewerPawnPtr->SpringArmComponent->TargetArmLength;

				return;
			}
		}
	}

	EndTask();
}

void UGT_ReplyCameraTransform::OnDestroy(
	bool bInOwnerFinished
	)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UGT_CameraTransformByPawnViewer::Activate()
{
	Super::Activate();

	if (ViewerPawnPtr)
	{
		TargetLocation = ViewerPawnPtr->GetActorLocation();
		TargetRotation = ViewerPawnPtr->GetActorRotation();
		TargetTargetArmLength = ViewerPawnPtr->SpringArmComponent->TargetArmLength;

		return;
	}

	EndTask();
}

void UGT_CameraTransformByPawnViewer::OnDestroy(
	bool bInOwnerFinished
	)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UPlayerControllerGameplayTasksComponent::OnGameplayTaskDeactivated(
	UGameplayTask& Task
	)
{
	Super::OnGameplayTaskDeactivated(Task);

	auto GTPtr = Cast<UGameplayTaskBase>(&Task);
	if (GTPtr && GTPtr->NextTaskPtr)
	{
		GTPtr->NextTaskPtr->ReadyForActivation();
	}
}

void UGameplayTaskBase::OnDestroy(
	bool bInOwnerFinished
	)
{
	OnTaskComplete.Broadcast(true);

	Super::OnDestroy(bInOwnerFinished);
}

UGT_CameraTransform::UGT_CameraTransform(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGT_CameraTransform::Activate()
{
	Super::Activate();

	auto PCPtr = GEngine->GetFirstLocalPlayerController(GetWorld());
	auto PawnPtr = Cast<ATourPawn>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn());
	if (PawnPtr)
	{
		OriginalLocation = PawnPtr->GetActorLocation();
		OriginalRotation = PCPtr->GetControlRotation();
		OriginalSpringArmLen = PawnPtr->SpringArmComponent->TargetArmLength;

		return;
	}

	EndTask();
}

void UGT_CameraTransform::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	if (CurrentTime < Duration)
	{
		CurrentTime += DeltaTime;

		const auto Percent = CurrentTime / Duration;
		Adjust(FMath::Clamp(Percent, 0.f, 1.f));
	}
	else
	{
		Adjust(1.f);
		EndTask();
	}
}

void UGT_CameraTransform::Adjust(
	float Percent
	) const
{
	auto PawnPtr = Cast<ATourPawn>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn());
	if (!PawnPtr)
	{
		return;
	}

	const auto CurrentLocation = UKismetMathLibrary::VLerp(
	                                                       OriginalLocation,
	                                                       TargetLocation,
	                                                       Percent
	                                                      );

	const auto CurrentRotation = UKismetMathLibrary::RLerp(
	                                                       OriginalRotation,
	                                                       TargetRotation,
	                                                       Percent,
	                                                       true
	                                                      );

	const auto CurrentTargetArmLength = FMath::Lerp(
	                                                OriginalSpringArmLen,
	                                                TargetTargetArmLength,
	                                                Percent
	                                               );

	PawnPtr->LerpToSeat(
	                    FTransform(
	                               CurrentRotation,
	                               CurrentLocation
	                              ),
	                    CurrentTargetArmLength
	                   );
}

void UGT_ModifyCameraTransform::Activate()
{
	Super::Activate();

	if (TargetTargetArmLength > 10.f)
	{
		return;
	}

	EndTask();
}

void UGT_ModifyCameraTransform::OnDestroy(
	bool bInOwnerFinished
	)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UGT_CameraTransformLocaterByID::Activate()
{
	Super::Activate();

	if (TargetDevicePtr.IsValid())
	{
	}
	else
	{
		TargetDevicePtr = USceneInteractionWorldSystem::GetInstance()->FindSceneActor(ID).Get();
	}

	if (!TargetDevicePtr.IsValid())
	{
		return;
	}

	auto Result = TargetDevicePtr->GetViewSeat();

	TargetLocation = Result.Key.GetLocation();
	TargetRotation = Result.Key.GetRotation().Rotator();
	TargetTargetArmLength = Result.Value + 500;
}

void UGT_CameraTransformLocaterBySpace::Activate()
{
	Super::Activate();

	auto TargetPtr = SpaceActorPtr;
	if (TargetPtr.IsValid())
	{
		auto Result = UKismetAlgorithm::GetCameraSeat(
		                                              {TargetPtr.Get()},
		                                              UGameOptions::GetInstance()->ViewSpaceRot,
		                                              UGameOptions::GetInstance()->ViewSpaceControlParam.FOV
		                                             );

		TargetLocation = Result.Key.GetLocation();
		TargetRotation = Result.Key.GetRotation().Rotator();
		TargetTargetArmLength = UGameOptions::GetInstance()->ViewSpaceArmLen;
	}
}

UGT_BatchBase::UGT_BatchBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
}

void UGT_BatchBase::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	switch (UseScopeType)
	{
	case EUseScopeType::kTime:
		{
			double TotalTime = 0.;
			for (;;)
			{
				double InScopeSeconds = 0.;
				{
					FSimpleScopeSecondsCounter SimpleScopeSecondsCounter(InScopeSeconds);

					if (ProcessTask(DeltaTime))
					{
					}
					else
					{
						break;
					}
				}
				TotalTime += InScopeSeconds;

				// PRINTINVOKEWITHSTR(FString::Printf(TEXT("InScopeSeconds %.2lf"), InScopeSeconds));
				// PRINTINVOKEWITHSTR(FString::Printf(TEXT("TotalTime %.2lf"), TotalTime));

				if (TotalTime > ScopeTime)
				{
					return;
				}

				if (UseScopeType != EUseScopeType::kTime)
				{
					return;
				}
			}
		}
		break;
	case EUseScopeType::kCount:
		{
			int32 CurrentTickProcessNum = 0;
			for (;;)
			{
				if (ProcessTask(DeltaTime))
				{
					CurrentTickProcessNum++;
					if (CurrentTickProcessNum < PerTickProcessNum)
					{
					}
					else
					{
						CurrentTickProcessNum = 0;
						return;
					}

					if (UseScopeType != EUseScopeType::kCount)
					{
						return;
					}
				}
				else
				{
					break;
				}
			}
		}
		break;
	case EUseScopeType::kNone:
		{
			if (ProcessTask(DeltaTime))
			{
				return;
			}
			else
			{
				break;
			}
		}
		break;
	}

	EndTask();
}

bool UGT_BatchBase::ProcessTask(
	float DeltaTime
	)
{
	return false;
}

UGT_InitializeSceneActors::UGT_InitializeSceneActors(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;

	Priority = FGameplayTasks::DefaultPriority;

	UseScopeType = EUseScopeType::kCount;

	PerTickProcessNum = 1000;
}

void UGT_InitializeSceneActors::Activate()
{
	Super::Activate();

	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		ProcessTask_ControlBorder(Iter.Value);

		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_AS;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_StructItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_ID;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_InnerStructItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_Soft;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_SoftDecorationItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_Space;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_SpaceItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->BuildingHelpers)
	{
		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_AS;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_StructItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_ID;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_InnerStructItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_Soft;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_SoftDecorationItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
		{
			auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
			SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_Space;

			Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
			                                                          SceneElementCategoryPtr->Category,
			                                                          SceneElementCategoryPtr
			                                                         );

			SceneElementCategoryPtr->AttachToActor(
			                                       Iter.Value.LoadSynchronous(),
			                                       FAttachmentTransformRules::KeepWorldTransform
			                                      );

			ProcessTask_SpaceItemSet(SceneElementCategoryPtr, Iter.Value->AllReference);
		}
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		auto SceneElementCategoryPtr = GetWorld()->SpawnActor<ASceneElementCategory>();
		SceneElementCategoryPtr->Category = USmartCitySuiteTags::SceneElement_Category_Space;

		Iter.Value.LoadSynchronous()->SceneElementCategoryMap.Add(
		                                                          SceneElementCategoryPtr->Category,
		                                                          SceneElementCategoryPtr
		                                                         );

		SceneElementCategoryPtr->AttachToActor(
		                                       Iter.Value.LoadSynchronous(),
		                                       FAttachmentTransformRules::KeepWorldTransform
		                                      );

		ProcessTask_Lanscape(SceneElementCategoryPtr, Iter.Value->AllReference.StructItemSet.DatasmithSceneActorSet);
		ProcessTask_Lanscape(
		                     SceneElementCategoryPtr,
		                     Iter.Value->AllReference.InnerStructItemSet.DatasmithSceneActorSet
		                    );
		ProcessTask_Lanscape(
		                     SceneElementCategoryPtr,
		                     Iter.Value->AllReference.SoftDecorationItem.DatasmithSceneActorSet
		                    );
		ProcessTask_Lanscape(SceneElementCategoryPtr, Iter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet);
	}

	OnEnd.Broadcast(true);

	EndTask();
}

void UGT_InitializeSceneActors::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);
}

void UGT_InitializeSceneActors::OnDestroy(
	bool bInOwnerFinished
	)
{
	OnEnd.Broadcast(true);

	Super::OnDestroy(bInOwnerFinished);
}

void UGT_InitializeSceneActors::ProcessTask_ControlBorder(
	const TSoftObjectPtr<AFloorHelper>& FloorHelperRef
	)
{
	ASceneElement_ControlBorder* SceneElement_ControlBorderPtr = nullptr;
	for (const auto& ControlBorderIter : FloorHelperRef.LoadSynchronous()->ControlBorderMaps)
	{
		{
			auto Components = ControlBorderIter.LoadSynchronous()->P2->GetComponents();
			for (auto SecondIter : Components)
			{
				auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
				if (!InterfacePtr)
				{
					continue;
				}
				auto AUDPtr = Cast<UDatasmithAssetUserData>(
				                                            InterfacePtr->GetAssetUserDataOfClass(
					                                             UDatasmithAssetUserData::StaticClass()
					                                            )
				                                           );
				if (!AUDPtr)
				{
					continue;
				}

				auto Datasmith_UniqueId = AUDPtr->MetaData.Find(TEXT("Datasmith_UniqueId"));
				if (!Datasmith_UniqueId)
				{
					continue;
				}

				SceneElement_ControlBorderPtr = GetWorld()->SpawnActor<ASceneElement_ControlBorder>();

				SceneElement_ControlBorderPtr->Replace(ControlBorderIter.LoadSynchronous()->P2, {}, AUDPtr->MetaData);
				SceneElement_ControlBorderPtr->InitialSceneElement();
				SceneElement_ControlBorderPtr->SceneElementID = *Datasmith_UniqueId;

				USceneInteractionWorldSystem::GetInstance()->SceneElementMap.
				                                             Add(
				                                                 SceneElement_ControlBorderPtr->SceneElementID,
				                                                 SceneElement_ControlBorderPtr
				                                                );
				break;
			}
		}
		{
			TMap<int32, ASceneElementBase*> MergeActorsMap;

			for (auto DSIter : ControlBorderIter.LoadSynchronous()->Devices)
			{
				bool bIsSceneElement = false;
				auto Components = DSIter->GetComponents();
				for (auto SecondIter : Components)
				{
					auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
					if (!InterfacePtr)
					{
						continue;
					}
					auto AUDPtr = Cast<UDatasmithAssetUserData>(
					                                            InterfacePtr->GetAssetUserDataOfClass(
						                                             UDatasmithAssetUserData::StaticClass()
						                                            )
					                                           );
					if (!AUDPtr)
					{
						continue;
					}

					auto Datasmith_UniqueId = AUDPtr->MetaData.Find(TEXT("Datasmith_UniqueId"));
					if (!Datasmith_UniqueId)
					{
						continue;
					}

					for (const auto& ThirdIter : UAssetRefMap::GetInstance()->NeedReplaceByUserData)
					{
						auto MetaDataIter = AUDPtr->MetaData.Find(*ThirdIter.Key.Key);
						if (!MetaDataIter)
						{
							continue;
						}
						if (ThirdIter.Key.bOnlyKey)
						{
						}
						else if (ThirdIter.Key.bSkip)
						{
							bIsSceneElement = true;
							continue;
						}
						else
						{
							if (*MetaDataIter != ThirdIter.Key.Value)
							{
								continue;
							}
						}

						{
							auto NewActorPtr = GetWorld()->SpawnActor<ASceneElementBase>(
								 ThirdIter.Value
								);
							NewActorPtr->Replace(DSIter, {*ThirdIter.Key.Key, *MetaDataIter}, AUDPtr->MetaData);

							// NewActorPtr->AttachToActor(SceneElement_ControlBorderPtr, FAttachmentTransformRules::KeepWorldTransform);

							NewActorPtr->InitialSceneElement();
							NewActorPtr->SceneElementID = *Datasmith_UniqueId;

							USceneInteractionWorldSystem::GetInstance()->SceneElementMap.
							                                             Add(
							                                                 NewActorPtr->SceneElementID,
							                                                 NewActorPtr
							                                                );

							SceneElement_ControlBorderPtr->ControlDevicesSet.Add(NewActorPtr);
						}
						bIsSceneElement = true;
						break;
					}

					for (const auto& ThirdIter : UAssetRefMap::GetInstance()->NeedMergeByUserData)
					{
						auto MetaDataIter = AUDPtr->MetaData.Find(*ThirdIter.Key.Key);
						if (!MetaDataIter)
						{
							continue;
						}

						if (ThirdIter.Key.bOnlyKey)
						{
						}
						else
						{
							if (*MetaDataIter != ThirdIter.Key.Value)
							{
								continue;
							}
						}

						auto HashCode = HashCombine(
						                            GetTypeHash(*MetaDataIter),
						                            GetTypeHash(ThirdIter.Key)
						                           );

						if (MergeActorsMap.Contains(HashCode))
						{
							MergeActorsMap[HashCode]->Merge(
							                                DSIter,
							                                {*ThirdIter.Key.Key, *MetaDataIter},
							                                AUDPtr->MetaData
							                               );
						}
						else
						{
							auto NewActorPtr = GetWorld()->SpawnActor<ASceneElementBase>(
								 ThirdIter.Value
								);
							NewActorPtr->Merge(DSIter, {*ThirdIter.Key.Key, *MetaDataIter}, AUDPtr->MetaData);

							// NewActorPtr->AttachToActor(SceneElement_ControlBorderPtr, FAttachmentTransformRules::KeepWorldTransform);

							NewActorPtr->InitialSceneElement();


							MergeActorsMap.Add(HashCode, NewActorPtr);
							USceneInteractionWorldSystem::GetInstance()->SceneElementMap.
							                                             Add(
							                                                 NewActorPtr->SceneElementID,
							                                                 NewActorPtr
							                                                );

							SceneElement_ControlBorderPtr->ControlDevicesSet.Add(NewActorPtr);
						}

						bIsSceneElement = true;
						break;
					}

					if (bIsSceneElement)
					{
						break;
					}
				}

				if (bIsSceneElement)
				{
				}
				else
				{
					if (DSIter->IsA(ASceneElementBase::StaticClass()))
					{
						continue;
					}

					if (DSIter->IsA(AStaticMeshActor::StaticClass()))
					{
						auto NewActorPtr = GetWorld()->SpawnActor<ASceneElement_Regualar>(
							);
						NewActorPtr->Replace(DSIter, {}, {});

						// NewActorPtr->AttachToActor(SceneElement_ControlBorderPtr, FAttachmentTransformRules::KeepWorldTransform);

						NewActorPtr->InitialSceneElement();

						SceneElement_ControlBorderPtr->ControlDevicesSet.Add(NewActorPtr);
					}
					else
					{
						auto NewActorPtr = GetWorld()->SpawnActor<ASceneElement_Regualar>(
							);
						NewActorPtr->Replace(DSIter, {}, {});

						// NewActorPtr->AttachToActor(SceneElement_ControlBorderPtr, FAttachmentTransformRules::KeepWorldTransform);

						NewActorPtr->InitialSceneElement();

						SceneElement_ControlBorderPtr->ControlDevicesSet.Add(NewActorPtr);
					}
				}
			}
		}
		{
			for (auto DSIter : ControlBorderIter.LoadSynchronous()->CurtainWall)
			{
				auto Components = DSIter->GetComponents();
				for (auto SecondIter : Components)
				{
					auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
					if (!InterfacePtr)
					{
						continue;
					}
					auto AUDPtr = Cast<UDatasmithAssetUserData>(
					                                            InterfacePtr->GetAssetUserDataOfClass(
						                                             UDatasmithAssetUserData::StaticClass()
						                                            )
					                                           );
					if (!AUDPtr)
					{
						continue;
					}

					auto Datasmith_UniqueId = AUDPtr->MetaData.Find(TEXT("Datasmith_UniqueId"));
					if (!Datasmith_UniqueId)
					{
						continue;
					}

					{
						auto NewActorPtr = GetWorld()->SpawnActor<ABuilding_CurtainWall>();

						NewActorPtr->GenerateRollerBlind(DSIter.LoadSynchronous());

						NewActorPtr->Replace(DSIter, {}, AUDPtr->MetaData);

						NewActorPtr->InitialSceneElement();

						NewActorPtr->SceneElementID = *Datasmith_UniqueId;

						USceneInteractionWorldSystem::GetInstance()->SceneElementMap.
						                                             Add(
						                                                 NewActorPtr->SceneElementID,
						                                                 NewActorPtr
						                                                );

						SceneElement_ControlBorderPtr->ControlDevicesSet.Add(NewActorPtr);
					}

					break;
				}
			}
		}
	}
}

bool UGT_InitializeSceneActors::ProcessTask_StructItemSet(
	ASceneElementCategory* SceneElementCategoryPtr,
	FSceneElementMap& AllReference
	)
{
	return ProcessTask_Generic(SceneElementCategoryPtr, AllReference, AllReference.StructItemSet);
}

bool UGT_InitializeSceneActors::ProcessTask_InnerStructItemSet(
	ASceneElementCategory* SceneElementCategoryPtr,
	FSceneElementMap& AllReference
	)
{
	return ProcessTask_Generic(SceneElementCategoryPtr, AllReference, AllReference.InnerStructItemSet);
}

bool UGT_InitializeSceneActors::ProcessTask_SoftDecorationItemSet(
	ASceneElementCategory* SceneElementCategoryPtr,
	FSceneElementMap& AllReference
	)
{
	return ProcessTask_Generic(SceneElementCategoryPtr, AllReference, AllReference.SoftDecorationItem);
}

bool UGT_InitializeSceneActors::ProcessTask_SpaceItemSet(
	ASceneElementCategory* SceneElementCategoryPtr,
	FSceneElementMap& AllReference
	)
{
	const auto SpaceInfo = UAssetRefMap::GetInstance()->SpaceInfo;

	for (const auto& DSIter : AllReference.SpaceItemSet.DatasmithSceneActorSet)
	{
		DSIter->AttachToActor(SceneElementCategoryPtr, FAttachmentTransformRules::KeepWorldTransform);

		TArray<AActor*> OutActors;

		DSIter.LoadSynchronous()->GetAttachedActors(OutActors, true, true);

		{
			TArray<AActor*> TempOutActors;

			DSIter.LoadSynchronous()->GetAttachedActors(TempOutActors, true);
			for (auto Iter : TempOutActors)
			{
				Iter->AttachToActor(SceneElementCategoryPtr, FAttachmentTransformRules::KeepWorldTransform);
			}
		}

		TMap<int32, ASceneElementBase*> MergeActorsMap;

		for (auto Iter : OutActors)
		{
			if (Iter)
			{
				auto Components = Iter->GetComponents();
				for (auto SecondIter : Components)
				{
					auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
					if (!InterfacePtr)
					{
						continue;
					}
					auto AUDPtr = Cast<UDatasmithAssetUserData>(
					                                            InterfacePtr->GetAssetUserDataOfClass(
						                                             UDatasmithAssetUserData::StaticClass()
						                                            )
					                                           );
					if (!AUDPtr)
					{
						continue;
					}

					auto Datasmith_UniqueId = AUDPtr->MetaData.Find(TEXT("Datasmith_UniqueId"));
					if (!Datasmith_UniqueId)
					{
						continue;
					}

					auto MetaDataIter = AUDPtr->MetaData.Find(*SpaceInfo.Key);
					if (!MetaDataIter)
					{
						continue;
					}
					if (*MetaDataIter != SpaceInfo.Value)
					{
						continue;
					}

					auto SpaceNameValueIter = AUDPtr->MetaData.Find(*SpaceInfo.SpaceNameValue);
					if (!SpaceNameValueIter)
					{
						continue;
					}

					auto HashCode = HashCombine(
					                            GetTypeHash(*MetaDataIter),
					                            GetTypeHash(*SpaceNameValueIter)
					                           );

					if (MergeActorsMap.Contains(HashCode))
					{
						MergeActorsMap[HashCode]->Merge(Iter, {*SpaceInfo.Key, SpaceInfo.Value}, AUDPtr->MetaData);
					}
					else
					{
						auto NewActorPtr = GetWorld()->SpawnActor<ASceneElement_Space>(
							 UAssetRefMap::GetInstance()->
							 SceneElement_SpaceClass
							);
						NewActorPtr->Merge(Iter, {*SpaceInfo.Key, SpaceInfo.Value}, AUDPtr->MetaData);
						NewActorPtr->InitialSceneElement();
						NewActorPtr->SceneElementID = *Datasmith_UniqueId;

						MergeActorsMap.Add(HashCode, NewActorPtr);
					}
					break;
				}
			}
		}

		DSIter->Destroy();
	}
	AllReference.SpaceItemSet.DatasmithSceneActorSet.Empty();

	return true;
}

bool UGT_InitializeSceneActors::ProcessTask_Lanscape(
	ASceneElementCategory* SceneElementCategoryPtr,
	TSet<TSoftObjectPtr<ADatasmithSceneActor>>& DatasmithSceneActorSet
	)
{
	for (const auto& DSIter : DatasmithSceneActorSet)
	{
		DSIter->AttachToActor(SceneElementCategoryPtr, FAttachmentTransformRules::KeepWorldTransform);

		TArray<AActor*> OutActors;

		DSIter.LoadSynchronous()->GetAttachedActors(OutActors, true, true);

		{
			TArray<AActor*> TempOutActors;

			DSIter.LoadSynchronous()->GetAttachedActors(TempOutActors, true);
			for (auto Iter : TempOutActors)
			{
				Iter->AttachToActor(SceneElementCategoryPtr, FAttachmentTransformRules::KeepWorldTransform);
			}
		}

		for (auto& Iter : OutActors)
		{
			auto NewActorPtr = GetWorld()->SpawnActor<ASceneElement_LandScape>(
			                                                                  );
			NewActorPtr->Replace(Iter, {}, {});
			NewActorPtr->InitialSceneElement();
		}

		DSIter->Destroy();
	}
	DatasmithSceneActorSet.Empty();

	return false;
}

bool UGT_InitializeSceneActors::ProcessTask_Generic(
	ASceneElementCategory* SceneElementCategoryPtr,
	FSceneElementMap& AllReference,
	FSceneElementFilter& StructItemSet
	)
{
	for (const auto& DSIter : StructItemSet.DatasmithSceneActorSet)
	{
		DSIter->AttachToActor(SceneElementCategoryPtr, FAttachmentTransformRules::KeepWorldTransform);

		TArray<AActor*> OutActors;

		DSIter.LoadSynchronous()->GetAttachedActors(OutActors, true, true);

		{
			TArray<AActor*> TempOutActors;

			DSIter.LoadSynchronous()->GetAttachedActors(TempOutActors, true);
			for (auto Iter : TempOutActors)
			{
				Iter->AttachToActor(SceneElementCategoryPtr, FAttachmentTransformRules::KeepWorldTransform);
			}
		}

		TMap<int32, ASceneElementBase*> MergeActorsMap;
		for (auto& Iter : OutActors)
		{
#if WITH_EDITOR
			const auto SceneElementName = Iter->GetActorLabel();

			if (SceneElementName == TEXT("墙_幕墙_幕墙_58"))
			{
				// checkNoEntry();
			}
#endif // WITH_EDITOR

			if (!IsValid(Iter))
			{
				PRINTINVOKEINFO();
				continue;
			}

			bool bIsSceneElement = false;
			auto Components = Iter->GetComponents();
			for (auto SecondIter : Components)
			{
				auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
				if (!InterfacePtr)
				{
					continue;
				}
				auto AUDPtr = Cast<UDatasmithAssetUserData>(
				                                            InterfacePtr->GetAssetUserDataOfClass(
					                                             UDatasmithAssetUserData::StaticClass()
					                                            )
				                                           );
				if (!AUDPtr)
				{
					continue;
				}

				auto Datasmith_UniqueId = AUDPtr->MetaData.Find(TEXT("Datasmith_UniqueId"));
				if (!Datasmith_UniqueId)
				{
					continue;
				}

				for (const auto& ThirdIter : UAssetRefMap::GetInstance()->NeedReplaceByUserData)
				{
					auto MetaDataIter = AUDPtr->MetaData.Find(*ThirdIter.Key.Key);
					if (!MetaDataIter)
					{
						continue;
					}
					if (ThirdIter.Key.bOnlyKey)
					{
					}
					else if (ThirdIter.Key.bSkip)
					{
						bIsSceneElement = true;
						continue;
					}
					else
					{
						if (*MetaDataIter != ThirdIter.Key.Value)
						{
							continue;
						}
					}

					{
						auto NewActorPtr = GetWorld()->SpawnActor<ASceneElementBase>(
							 ThirdIter.Value
							);
						NewActorPtr->Replace(Iter, {*ThirdIter.Key.Key, *MetaDataIter}, AUDPtr->MetaData);
						NewActorPtr->InitialSceneElement();
						NewActorPtr->SceneElementID = *Datasmith_UniqueId;

						USceneInteractionWorldSystem::GetInstance()->SceneElementMap.
						                                             Add(NewActorPtr->SceneElementID, NewActorPtr);
					}
					bIsSceneElement = true;
					break;
				}

				for (const auto& ThirdIter : UAssetRefMap::GetInstance()->NeedMergeByUserData)
				{
					auto MetaDataIter = AUDPtr->MetaData.Find(*ThirdIter.Key.Key);
					if (!MetaDataIter)
					{
						continue;
					}

					if (ThirdIter.Key.bOnlyKey)
					{
					}
					else
					{
						if (*MetaDataIter != ThirdIter.Key.Value)
						{
							continue;
						}
					}

					auto HashCode = HashCombine(
					                            GetTypeHash(*MetaDataIter),
					                            GetTypeHash(ThirdIter.Key)
					                           );

					if (MergeActorsMap.Contains(HashCode))
					{
						MergeActorsMap[HashCode]->Merge(Iter, {*ThirdIter.Key.Key, *MetaDataIter}, AUDPtr->MetaData);
					}
					else
					{
						auto NewActorPtr = GetWorld()->SpawnActor<ASceneElementBase>(
							 ThirdIter.Value
							);
						NewActorPtr->Merge(Iter, {*ThirdIter.Key.Key, *MetaDataIter}, AUDPtr->MetaData);
						NewActorPtr->InitialSceneElement();
						if (ThirdIter.Value == ASceneElement_PWR_Pipe::StaticClass())
						{
						}
						NewActorPtr->InitialSceneElement();

						MergeActorsMap.Add(HashCode, NewActorPtr);
						USceneInteractionWorldSystem::GetInstance()->SceneElementMap.
						                                             Add(NewActorPtr->SceneElementID, NewActorPtr);
					}

					bIsSceneElement = true;
					break;
				}

				if (bIsSceneElement)
				{
					break;
				}
			}

			if (bIsSceneElement)
			{
			}
			else
			{
				if (Iter->IsA(ASceneElementBase::StaticClass()))
				{
					continue;
				}

				if (Iter->IsA(AStaticMeshActor::StaticClass()))
				{
					auto NewActorPtr = GetWorld()->SpawnActor<ASceneElement_Regualar>(
						);
					NewActorPtr->Replace(Iter, {}, {});
					NewActorPtr->InitialSceneElement();
				}
				else
				{
					auto NewActorPtr = GetWorld()->SpawnActor<ASceneElement_Regualar>(
						);
					NewActorPtr->Replace(Iter, {}, {});
					NewActorPtr->InitialSceneElement();
				}
			}
		}

		DSIter->Destroy();
	}
	StructItemSet.DatasmithSceneActorSet.Empty();

	return false;
}

UGT_SwitchSceneElement_Base::UGT_SwitchSceneElement_Base(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;

	Priority = 1.5 * FGameplayTasks::DefaultPriority;
}

void UGT_SwitchSceneElement_Base::Activate()
{
	Super::Activate();

	FilterTags.ConditionalSet.RemoveTag(USmartCitySuiteTags::Interaction_Mode_Empty);
}

void UGT_SwitchSceneElement_Base::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);
}

void UGT_SwitchSceneElement_Base::OnDestroy(
	bool bInOwnerFinished
	)
{
	Super::OnDestroy(bInOwnerFinished);
}

bool UGT_SwitchSceneElement_Base::ProcessTask(
	float DeltaTime
	)
{
	switch (Step)
	{
	case EStep::kDisplay:
		{
			if (ProcessTask_Display())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kHiden;
			return true;
		}
		break;
	case EStep::kHiden:
		{
			if (ProcessTask_Hiden())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kConfirmConditional;
			return true;
		}
		break;
	case EStep::kConfirmConditional:
		{
			if (ProcessTask_ConfirmConditional())
			{
				return true;
			}
			else
			{
			}

			UseScopeType = EUseScopeType::kCount;
			PerTickProcessNum = 500;

			Step = EStep::kSwitchState;
			return true;
		}
		break;
	case EStep::kSwitchState:
		{
			if (ProcessTask_SwitchState())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kSwitchState_Elevator;
			return true;
		}
	case EStep::kSwitchState_Elevator:
		{
			if (ProcessTask_SwitchState_Elevator())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kComplete;
			return true;
		}
	case EStep::kComplete:
	default:
		{
		}
	}

	if (OnEnd.IsBound())
	{
		OnEnd.Broadcast(true, this);
	}

	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_Display()
{
	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_Hiden()
{
	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_ConfirmConditional()
{
	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_SwitchState()
{
	if (DisplayAryIndex < NeedDisplayAry.Num())
	{
		ON_SCOPE_EXIT
		{
			DisplayAryIndex++;
		};

		auto ActorPtr = NeedDisplayAry[DisplayAryIndex];
		if (ActorPtr)
		{
			auto SceneElementPtr = Cast<ASceneElementBase>(ActorPtr);
			if (SceneElementPtr)
			{
				// auto SceneElement_ControlBorderPtr = Cast<ASceneElement_ControlBorder>(
				// 	 SceneElementPtr->GetAttachParentActor()
				// 	);
				// if (SceneElement_ControlBorderPtr)
				// {
				// }
				// else
				// {
				SceneElementPtr->SwitchInteractionType(FilterTags);
				// }
			}
			else
			{
				ActorPtr->SetActorHiddenInGame(false);

				TArray<AActor*> OutActors;

				ActorPtr->GetAttachedActors(OutActors, true, true);
				for (auto Iter : OutActors)
				{
					Iter->SetActorHiddenInGame(false);
				}
			}
		}

		return true;
	}

	if (HideAryIndex < NeedHideAry.Num())
	{
		ON_SCOPE_EXIT
		{
			HideAryIndex++;
		};

		auto ActorPtr = NeedHideAry[HideAryIndex];
		if (ActorPtr)
		{
			auto SceneElementPtr = Cast<ASceneElementBase>(ActorPtr);
			if (SceneElementPtr)
			{
				// auto SceneElement_ControlBorderPtr = Cast<ASceneElement_ControlBorder>(
				// 	 SceneElementPtr->GetAttachParentActor()
				// 	);
				// if (SceneElement_ControlBorderPtr)
				// {
				// }
				// else
				// {
				SceneElementPtr->SwitchInteractionType(FSceneElementConditional::EmptyConditional);
				// }
			}
			else
			{
				ActorPtr->SetActorHiddenInGame(true);

				TArray<AActor*> OutActors;

				ActorPtr->GetAttachedActors(OutActors, true, true);
				for (auto Iter : OutActors)
				{
					Iter->SetActorHiddenInGame(true);
				}
			}
		}

		return true;
	}

	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_SwitchState_Elevator()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->ElevatorMap)
	{
		if (FloorIter.Value)
		{
			FloorIter.Value->SwitchInteractionType(FilterTags);
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_Tower::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		BuildingsAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_AS))
			{
				TArray<AActor*> OutActors;

				Iter.Value->GetAttachedActors(OutActors, true, true);

				for (auto ActorIter : OutActors)
				{
					auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
					if (SceneElementBasePtr)
					{
						NeedDisplayAry.Add(SceneElementBasePtr);
					}
				}
			}
			else if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_ID))
			{
				TArray<AActor*> OutActors;

				Iter.Value->GetAttachedActors(OutActors, true, true);

				for (auto ActorIter : OutActors)
				{
					auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
					if (SceneElementBasePtr)
					{
						NeedDisplayAry.Add(SceneElementBasePtr);
					}
				}
			}
		}
	}

	for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->BuildingHelpers)
	{
		BuildingsAry.Add(LandScapeIter.Value.LoadSynchronous());
		for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedDisplayAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		BuildingsAry.Add(LandScapeIter.Value.LoadSynchronous());
		for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedDisplayAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_Tower::ProcessTask_Hiden()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_Soft))
			{
				TArray<AActor*> OutActors;

				Iter.Value->GetAttachedActors(OutActors, true, true);

				for (auto ActorIter : OutActors)
				{
					auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
					if (SceneElementBasePtr)
					{
						NeedHideAry.Add(SceneElementBasePtr);
					}
				}
			}
			else if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_Space))
			{
				TArray<AActor*> OutActors;

				Iter.Value->GetAttachedActors(OutActors, true, true);

				for (auto ActorIter : OutActors)
				{
					auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
					if (SceneElementBasePtr)
					{
						NeedHideAry.Add(SceneElementBasePtr);
					}
				}
			}
		}
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
	}

	return false;
}

bool UGT_SwitchSceneElement_Tower::ProcessTask_ConfirmConditional()
{
	return false;
}

bool UGT_SwitchSceneElement_Tower::ProcessTask_SwitchState()
{
	const auto Value = Super::ProcessTask_SwitchState();
	if (Value)
	{
		return true;
	}
	else
	{
		if (BuildingsAryIndex < BuildingsAry.Num())
		{
			ON_SCOPE_EXIT
			{
				BuildingsAryIndex++;
			};

			auto ActorPtr = BuildingsAry[BuildingsAryIndex];
			if (ActorPtr)
			{
				auto SceneElementPtr = Cast<ASceneElementBase>(ActorPtr);
				if (SceneElementPtr)
				{
					SceneElementPtr->SwitchInteractionType(FilterTags);
				}
				else
				{
					ActorPtr->SetActorHiddenInGame(false);

					TArray<AActor*> OutActors;

					ActorPtr->GetAttachedActors(OutActors, true, true);
					for (auto Iter : OutActors)
					{
						Iter->SetActorHiddenInGame(false);
					}
				}
			}

			return true;
		}
	}
	return false;
}

bool UGT_SwitchSceneElement_Tower::ProcessTask_SwitchState_Elevator()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->ElevatorMap)
	{
		if (FloorIter.Value)
		{
			FloorIter.Value->SwitchInteractionType(FilterTags);
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_Floor::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
		{
		}
		else
		{
			continue;
		}

		NeedDisplayAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					if (SkipSceneElementSet.Contains(SceneElementBasePtr))
					{
						PRINTFUNC();
					}
					else
					{
						NeedDisplayAry.Add(SceneElementBasePtr);
					}
				}
			}
		}
	}
	return false;
}

bool UGT_SwitchSceneElement_Floor::ProcessTask_Hiden()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
		{
			continue;
		}
		else
		{
		}

		NeedHideAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		NeedHideAry.Add(LandScapeIter.Value.LoadSynchronous());
		for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_Floor_JF::ProcessTask_Hiden()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->FloorTag.MatchesTagExact(USmartCitySuiteTags::Interaction_Area_Floor_F12))
		{
		}
		else
		{
			continue;
		}

		PriorityHideFloorAryAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					PriorityHideFloorAryAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	if (FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor))
	{
		for (const auto Iter : FilterTags.ConditionalSet)
		{
			if (Iter.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
			{
				FloorTag = Iter;
				break;
			}
		}
	}

	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
		{
			continue;
		}
		else
		{
		}

		if (FloorIter.Value->FloorTag.MatchesTagExact(USmartCitySuiteTags::Interaction_Area_Floor_F12))
		{
			continue;
		}
		else
		{
		}

		NeedHideAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		NeedHideAry.Add(LandScapeIter.Value.LoadSynchronous());
		for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_Floor_JF::ProcessTask_SwitchState()
{
	if (PriorityHideFloorAryIndex < PriorityHideFloorAryAry.Num())
	{
		ON_SCOPE_EXIT
		{
			PriorityHideFloorAryIndex++;
		};

		auto ActorPtr = PriorityHideFloorAryAry[PriorityHideFloorAryIndex];
		if (ActorPtr)
		{
			auto SceneElementPtr = Cast<ASceneElementBase>(ActorPtr);
			if (SceneElementPtr)
			{
				SceneElementPtr->SwitchInteractionType(FSceneElementConditional::EmptyConditional);
			}
			else
			{
				ActorPtr->SetActorHiddenInGame(true);

				TArray<AActor*> OutActors;

				ActorPtr->GetAttachedActors(OutActors, true, true);
				for (auto Iter : OutActors)
				{
					Iter->SetActorHiddenInGame(true);
				}
			}
		}

		return true;
	}

	return Super::ProcessTask_SwitchState();
}

void UGT_SwitchSceneElement_Space::Activate()
{
	Super::Activate();

	if (SceneElementPtr.IsValid())
	{
		FloorTag = SceneElementPtr->BelongFloor->FloorTag;
	}

	// FilterTags.ConditionalSet = FGameplayTagContainer{FloorTag};
}

bool UGT_SwitchSceneElement_Space::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
		{
		}
		else
		{
			continue;
		}

		NeedDisplayAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_Space))
			{
				TArray<AActor*> OutActors;

				Iter.Value->GetAttachedActors(OutActors, true, true);

				for (auto ActorIter : OutActors)
				{
					auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
					if (SceneElementBasePtr)
					{
						if (ActorIter == SceneElementPtr)
						{
						}
						else if (SkipSceneElementSet.Contains(SceneElementBasePtr))
						{
							PRINTFUNC();
						}
						else
						{
							NeedDisplayAry.Add(SceneElementBasePtr);
						}
					}
				}
				continue;
			}

			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					if (SkipSceneElementSet.Contains(SceneElementBasePtr))
					{
						PRINTFUNC();
					}
					else
					{
						NeedDisplayAry.Add(SceneElementBasePtr);
					}
				}
			}
		}
	}
	return false;
}

bool UGT_SwitchSceneElement_Space::ProcessTask_Hiden()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
		{
			continue;
		}
		else
		{
		}

		NeedHideAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		NeedHideAry.Add(LandScapeIter.Value.LoadSynchronous());
		for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_Space::ProcessTask_SwitchState()
{
	if (SceneElementPtr.IsValid())
	{
		FSceneElementConditional TempFilterTags;

		TempFilterTags.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_View);

		if (SceneElementPtr.IsValid())
		{
			SceneElementPtr->SwitchInteractionType(TempFilterTags);
		}

		SceneElementPtr = nullptr;
		return true;
	}

	if (Super::ProcessTask_SwitchState())
	{
		return true;
	}

	return false;
}

void UGT_SwitchSceneElement_Device::Activate()
{
	Super::Activate();

	FilterTags.ConditionalSet = FGameplayTagContainer{FloorTag};
}

bool UGT_SwitchSceneElement_Device::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
		{
		}
		else
		{
			continue;
		}

		NeedDisplayAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_Soft))
			{
				TArray<AActor*> OutActors;

				Iter.Value->GetAttachedActors(OutActors, true, true);

				for (auto ActorIter : OutActors)
				{
					auto SceneElementBasePtr = Cast<ASceneElement_DeviceBase>(ActorIter);
					if (SceneElementBasePtr)
					{
						if (SceneElementSet.Contains(SceneElementBasePtr))
						{
						}
						else
						{
							NeedDisplayAry.Add(SceneElementBasePtr);
						}
					}
				}
				continue;
			}

			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedDisplayAry.Add(SceneElementBasePtr);
				}
			}
		}
	}
	return false;
}

bool UGT_SwitchSceneElement_Device::ProcessTask_Hiden()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
		{
			continue;
		}
		else
		{
		}

		NeedHideAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		NeedHideAry.Add(LandScapeIter.Value.LoadSynchronous());
		for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_Device::ProcessTask_SwitchState()
{
	if (SceneElementSet.IsEmpty())
	{
		return Super::ProcessTask_SwitchState();
	}
	else
	{
		FSceneElementConditional TempFilterTags;

		TempFilterTags.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_View);
		// TempFilterTags.ConditionalSet.AddTag(FloorTag);

		for (auto Iter : SceneElementSet)
		{
			Iter->SwitchInteractionType(TempFilterTags);
		}
		SceneElementSet.Empty();
		return true;
	}
}

void UGT_SwitchSceneElement_BatchDevicesControl::Activate()
{
	Super::Activate();

	FilterTags.ConditionalSet = FGameplayTagContainer{FloorTag};
}

bool UGT_SwitchSceneElement_BatchDevicesControl::ProcessTask_Display()
{
	// for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	// {
	// 	if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
	// 	{
	// 	}
	// 	else
	// 	{
	// 		continue;
	// 	}
	//
	// 	NeedDisplayAry.Add(FloorIter.Value.LoadSynchronous());
	// 	for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
	// 	{
	// 		if (Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_Soft))
	// 		{
	// 			TArray<AActor*> OutActors;
	//
	// 			Iter.Value->GetAttachedActors(OutActors, true, true);
	//
	// 			for (auto ActorIter : OutActors)
	// 			{
	// 				auto SceneElementBasePtr = Cast<ASceneElement_DeviceBase>(ActorIter);
	// 				if (SceneElementBasePtr)
	// 				{
	// 					if (SceneElementSet.Contains(SceneElementBasePtr))
	// 					{
	// 						PRINTFUNC();
	// 					}
	// 					else
	// 					{
	// 						NeedDisplayAry.Add(SceneElementBasePtr);
	// 					}
	// 				}
	// 			}
	// 			continue;
	// 		}
	//
	// 		TArray<AActor*> OutActors;
	//
	// 		Iter.Value->GetAttachedActors(OutActors, true, true);
	//
	// 		for (auto ActorIter : OutActors)
	// 		{
	// 			auto SceneElementBasePtr = Cast<ASceneElement_DeviceBase>(ActorIter);
	// 			if (SceneElementBasePtr)
	// 			{
	// 				if (SceneElementSet.Contains(SceneElementBasePtr))
	// 				{
	// 					PRINTFUNC();
	// 				}
	// 				else
	// 				{
	// 					NeedDisplayAry.Add(SceneElementBasePtr);
	// 				}
	// 			}
	// 		}
	// 	}
	// }
	return false;
}

bool UGT_SwitchSceneElement_BatchDevicesControl::ProcessTask_Hiden()
{
	// for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	// {
	// 	if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
	// 	{
	// 		continue;
	// 	}
	// 	else
	// 	{
	// 	}
	//
	// 	NeedHideAry.Add(FloorIter.Value.LoadSynchronous());
	// 	for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
	// 	{
	// 		TArray<AActor*> OutActors;
	//
	// 		Iter.Value->GetAttachedActors(OutActors, true, true);
	//
	// 		for (auto ActorIter : OutActors)
	// 		{
	// 			auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
	// 			if (SceneElementBasePtr)
	// 			{
	// 				NeedHideAry.Add(SceneElementBasePtr);
	// 			}
	// 		}
	// 	}
	// }
	//
	// for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->LandScapeHelper)
	// {
	// 	NeedHideAry.Add(LandScapeIter.Value.LoadSynchronous());
	// 	for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
	// 	{
	// 		TArray<AActor*> OutActors;
	//
	// 		Iter.Value->GetAttachedActors(OutActors, true, true);
	//
	// 		for (auto ActorIter : OutActors)
	// 		{
	// 			auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
	// 			if (SceneElementBasePtr)
	// 			{
	// 				NeedHideAry.Add(SceneElementBasePtr);
	// 			}
	// 		}
	// 	}
	// }

	return false;
}

bool UGT_SwitchSceneElement_BatchDevicesControl::ProcessTask_SwitchState()
{
	if (SceneElementSet.IsEmpty())
	{
		return Super::ProcessTask_SwitchState();
	}
	else
	{
		FSceneElementConditional TempFilterTags;

		TempFilterTags.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_BatchControl);
		// TempFilterTags.ConditionalSet.AddTag(FloorTag);

		for (auto Iter : SceneElementSet)
		{
			Iter->SwitchInteractionType(TempFilterTags);
		}
		SceneElementSet.Empty();
		return true;
	}
}

bool UGT_SwitchSceneElement_SpecialArea::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorSet.Contains(FloorIter.Value->FloorTag))
		{
		}
		else
		{
			continue;
		}

		NeedDisplayAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedDisplayAry.Add(SceneElementBasePtr);
				}
			}
		}
	}
	return false;
}

bool UGT_SwitchSceneElement_SpecialArea::ProcessTask_Hiden()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (PriorityHideFloorSet.Contains(FloorIter.Value->FloorTag))
		{
		}
		else
		{
			continue;
		}

		PriorityHideFloorAryAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					PriorityHideFloorAryAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorSet.Contains(FloorIter.Value->FloorTag))
		{
			continue;
		}
		else
		{
		}

		if (PriorityHideFloorSet.Contains(FloorIter.Value->FloorTag))
		{
			continue;
		}
		else
		{
		}

		NeedHideAry.Add(FloorIter.Value.LoadSynchronous());
		for (const auto& Iter : FloorIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	for (const auto& LandScapeIter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		NeedHideAry.Add(LandScapeIter.Value.LoadSynchronous());
		for (const auto& Iter : LandScapeIter.Value.LoadSynchronous()->SceneElementCategoryMap)
		{
			TArray<AActor*> OutActors;

			Iter.Value->GetAttachedActors(OutActors, true, true);

			for (auto ActorIter : OutActors)
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(ActorIter);
				if (SceneElementBasePtr)
				{
					NeedHideAry.Add(SceneElementBasePtr);
				}
			}
		}
	}

	return false;
}

bool UGT_SwitchSceneElement_SpecialArea::ProcessTask_SwitchState()
{
	if (PriorityHideFloorAryIndex < PriorityHideFloorAryAry.Num())
	{
		ON_SCOPE_EXIT
		{
			PriorityHideFloorAryIndex++;
		};

		auto ActorPtr = PriorityHideFloorAryAry[PriorityHideFloorAryIndex];
		if (ActorPtr)
		{
			auto SceneElementPtr = Cast<ASceneElementBase>(ActorPtr);
			if (SceneElementPtr)
			{
				SceneElementPtr->SwitchInteractionType(FSceneElementConditional::EmptyConditional);
			}
			else
			{
				ActorPtr->SetActorHiddenInGame(true);

				TArray<AActor*> OutActors;

				ActorPtr->GetAttachedActors(OutActors, true, true);
				for (auto Iter : OutActors)
				{
					Iter->SetActorHiddenInGame(true);
				}
			}
		}

		return true;
	}

	return Super::ProcessTask_SwitchState();
}
