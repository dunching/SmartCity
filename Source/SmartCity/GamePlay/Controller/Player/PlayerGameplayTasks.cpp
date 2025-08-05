#include "PlayerGameplayTasks.h"

#include "DatasmithAssetUserData.h"
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
#include "ViewerPawn.h"
#include "Tools.h"
#include "GameplayTagsLibrary.h"
#include "AssetRefMap.h"
#include "DatasmithSceneActor.h"
#include "FloorHelper.h"
#include "GenerateTypes.h"
#include "LogWriter.h"
#include "ReplaceActor.h"
#include "SceneElementBase.h"
#include "TemplateHelper.h"
#include "CollisionDataStruct.h"
#include "SceneElement_RadarSweep.h"

struct FPrefix : public TStructVariable<FPrefix>
{
	FName Datasmith_UniqueId = TEXT("Datasmith_UniqueId");
};

FName UPlayerControllerGameplayTasksComponent::ComponentName = TEXT("PlayerControllerGameplayTasksComponent");

UGT_ReplyCameraTransform::UGT_ReplyCameraTransform(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGT_ReplyCameraTransform::Activate()
{
	Super::Activate();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(
	                                      this,
	                                      AViewerPawn::StaticClass(),
	                                      OutActors
	                                     );
	for (auto ActorIter : OutActors)
	{
		auto ViewerPawnPtr = Cast<AViewerPawn>(ActorIter);
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
	):
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

	auto TargetPtr = USceneInteractionWorldSystem::GetInstance()->FindSceneActor(ID);
	if (TargetPtr.IsValid())
	{
		auto Result = UKismetAlgorithm::GetCameraSeat(
		                                              {TargetPtr.Get()},
		                                              UGameOptions::GetInstance()->ViewDeviceRot,
		                                              UGameOptions::GetInstance()->ViewDeviceControlParam.FOV
		                                             );

		TargetLocation = Result.Key.GetLocation();
		TargetRotation = Result.Key.GetRotation().Rotator();
		TargetTargetArmLength = Result.Value;
	}
}

void UGT_CameraTransformLocaterBySpace::Activate()
{
	Super::Activate();

	auto TargetPtr = SpaceActorPtr;
	if (TargetPtr.IsValid())
	{
		auto Result = UKismetAlgorithm::GetCameraSeat(
		                                              {TargetPtr.Get()},
		                                              UGameOptions::GetInstance()->ViewDeviceRot,
		                                              UGameOptions::GetInstance()->ViewDeviceControlParam.FOV
		                                             );

		TargetLocation = Result.Key.GetLocation();
		TargetRotation = Result.Key.GetRotation().Rotator();
		TargetTargetArmLength = Result.Value;
	}
}

UGT_BatchBase::UGT_BatchBase(
	const FObjectInitializer& ObjectInitializer
	):
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
			double InScopeSeconds = 0.;
			for (;;)
			{
				FSimpleScopeSecondsCounter SimpleScopeSecondsCounter(InScopeSeconds);
				if (InScopeSeconds > 0.1)
				{
					InScopeSeconds = 0;
					return;
				}

				if (ProcessTask(DeltaTime))
				{
				}
				else
				{
					break;
				}
			}
		}
		break;
	case EUseScopeType::kCount:
		{
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
	):
	 Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;

	Priority = FGameplayTasks::DefaultPriority / 2;

	UseScopeType = EUseScopeType::kCount;

	PerTickProcessNum = 1000;
}

void UGT_InitializeSceneActors::Activate()
{
	Super::Activate();

	for (const auto& Iter : UAssetRefMap::GetInstance()->AllSceneActorMap)
	{
		SceneActorMap.Add(Iter);
	}

	ApplyData(0);
	StepIndex = -1;
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

bool UGT_InitializeSceneActors::ProcessTask(
	float DeltaTime
	)
{
	if (SceneActorMapIndex < SceneActorMap.Num())
	{
	}
	else
	{
		return false;
	}

	switch (StepIndex)
	{
	case -2:
		{
			if (ProcessTask_RecordFloor())
			{
				return true;
			}
			else
			{
				StepIndex++;
				return true;
			}
		}
	case -1:
		{
			if (ProcessTask_NeedReplaceByRef())
			{
				return true;
			}
			else
			{
				StepIndex++;
				return true;
			}
		}
	case 0:
		{
			if (ProcessTask_StructItemSet())
			{
				return true;
			}
			else
			{
				if (InnerStructItemSetIndex < InnerStructItemSet.Num())
				{
					ApplyRelatedActors(InnerStructItemSet[InnerStructItemSetIndex]);
				}
				else
				{
					RelatedActorsIndex = 0;
					RelatedActors.Empty();
				}

				StepIndex++;
				return true;
			}
		}
	case 1:
		{
			if (ProcessTask_InnerStructItemSet())
			{
				return true;
			}
			else
			{
				if (SoftDecorationItemSetIndex < SoftDecorationItemSet.Num())
				{
					ApplyRelatedActors(SoftDecorationItemSet[SoftDecorationItemSetIndex]);
				}
				else
				{
					RelatedActorsIndex = 0;
					RelatedActors.Empty();
				}

				StepIndex++;
				return true;
			}
		}
	case 2:
		{
			if (ProcessTask_SoftDecorationItemSet())
			{
				return true;
			}
			else
			{
				StepIndex++;
				return true;
			}
		}
	case 3:
		{
			StepIndex++;
			return true;
		}
	case 4:
		{
			if (ProcessTask_SpaceItemSet())
			{
				return true;
			}
		}
	default: ;
	}

	SceneActorMapIndex++;

	ApplyData(SceneActorMapIndex);

	return true;
}

bool UGT_InitializeSceneActors::ProcessTask_RecordFloor()
{
	return false;
}

bool UGT_InitializeSceneActors::ProcessTask_NeedReplaceByRef()
{
	auto NeedReplaceByRef = UAssetRefMap::GetInstance()->NeedReplaceByRef;

	for (auto Iter : NeedReplaceByRef)
	{
		TArray<AActor*> OutActors;
		Iter.Key->GetAttachedActors(OutActors, true, true);

		for (auto& SecondIter : OutActors)
		{
			auto NewActorPtr = GetWorld()->SpawnActor<ASceneElementBase>(
			                                                             Iter.Value,
			                                                             SecondIter->GetActorTransform()
			                                                            );
			NewActorPtr->Replace(SecondIter);
		}
	}

	return false;
}

bool UGT_InitializeSceneActors::ProcessTask_StructItemSet()
{
	if (StructItemSetIndex < StructItemSet.Num())
	{
	}
	else
	{
		return false;
	}

	if (NormalAdjust(StructItemSetIndex, StructItemSet))
	{
	}
	else
	{
		return false;
	}

	ON_SCOPE_EXIT
	{
		RelatedActorsIndex++;
	};

	auto Iter = RelatedActors[RelatedActorsIndex];
	if (Iter)
	{
		if (ReplacedActor(Iter))
		{
			return true;
		}

		auto Components = Iter->GetComponents();
		for (auto SecondIter : Components)
		{
			auto PrimitiveComponentPtr = Cast<UPrimitiveComponent>(SecondIter);
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				PrimitiveComponentPtr->SetCollisionObjectType(ExternalWall_Object);
				PrimitiveComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				PrimitiveComponentPtr->SetRenderCustomDepth(false);

				break;
			}
		}
	}

	return true;
}

bool UGT_InitializeSceneActors::ProcessTask_InnerStructItemSet()
{
	if (InnerStructItemSetIndex < InnerStructItemSet.Num())
	{
	}
	else
	{
		return false;
	}

	if (NormalAdjust(InnerStructItemSetIndex, InnerStructItemSet))
	{
	}
	else
	{
		return false;
	}

	ON_SCOPE_EXIT
	{
		RelatedActorsIndex++;
	};

	auto Iter = RelatedActors[RelatedActorsIndex];
	if (Iter)
	{
		if (ReplacedActor(Iter))
		{
			return true;
		}

		auto Components = Iter->GetComponents();
		for (auto SecondIter : Components)
		{
			auto PrimitiveComponentPtr = Cast<UPrimitiveComponent>(SecondIter);
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				PrimitiveComponentPtr->SetCollisionObjectType(Floor_Object);
				PrimitiveComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				PrimitiveComponentPtr->SetRenderCustomDepth(false);

				break;
			}
		}
	}

	return true;
}

bool UGT_InitializeSceneActors::ProcessTask_SoftDecorationItemSet()
{
	if (NormalAdjust(SoftDecorationItemSetIndex, SoftDecorationItemSet))
	{
	}
	else
	{
		return false;
	}

	ON_SCOPE_EXIT
	{
		RelatedActorsIndex++;
	};

	auto Iter = RelatedActors[RelatedActorsIndex];
	if (Iter)
	{
		if (ReplacedActor(Iter))
		{
			return true;
		}

		auto Components = Iter->GetComponents();
		for (auto SecondIter : Components)
		{
			auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
			if (InterfacePtr)
			{
				auto AUDPtr = Cast<UDatasmithAssetUserData>(
				                                            InterfacePtr->GetAssetUserDataOfClass(
					                                             UDatasmithAssetUserData::StaticClass()
					                                            )
				                                           );
				if (!AUDPtr)
				{
					continue;
				}
				for (const auto& ThirdIter : AUDPtr->MetaData)
				{
					if (ThirdIter.Key == UAssetRefMap::GetInstance()->Datasmith_UniqueId)
					{
						SceneInteractionWorldSystemPtr->ItemRefMap.Add(FGuid(ThirdIter.Value), Iter);
						continue;
					}
					else
					{
					}

					auto CatogoryPrefixIter = UAssetRefMap::GetInstance()->CatogoryPrifix.
					                                                       Find(ThirdIter.Key.ToString());
					if (CatogoryPrefixIter)
					{
						if (ThirdIter.Value == UAssetRefMap::GetInstance()->FJPG)
						{
							continue;
						}
						else if (ThirdIter.Value == UAssetRefMap::GetInstance()->XFJZ)
						{
							continue;
						}
						else
						{
							continue;
						}
					}
				}
				break;
			}
		}
		for (auto SecondIter : Components)
		{
			auto PrimitiveComponentPtr = Cast<UPrimitiveComponent>(SecondIter);
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				PrimitiveComponentPtr->SetCollisionObjectType(Device_Object);
				PrimitiveComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				PrimitiveComponentPtr->SetRenderCustomDepth(false);

				break;
			}
		}
	}

	return true;
}

bool UGT_InitializeSceneActors::ProcessTask_ReplaceSoftDecorationItemSet()
{
	return false;
}

bool UGT_InitializeSceneActors::ProcessTask_SpaceItemSet()
{
	if (NormalAdjust(SpaceItemSetIndex, SpaceItemSet))
	{
	}
	else
	{
		return false;
	}

	ON_SCOPE_EXIT
	{
		RelatedActorsIndex++;
	};

	auto SpaceMaterialInstance = UAssetRefMap::GetInstance()->SpaceMaterialInstance;
	auto Iter = RelatedActors[RelatedActorsIndex];
	if (Iter)
	{
		if (ReplacedActor(Iter))
		{
			return true;
		}

		auto Components = Iter->GetComponents();
		for (auto SecondIter : Components)
		{
			auto PrimitiveComponentPtr = Cast<UPrimitiveComponent>(SecondIter);
			if (PrimitiveComponentPtr)
			{
				for (int32 Index = 0; Index < PrimitiveComponentPtr->GetNumMaterials(); Index++)
				{
					PrimitiveComponentPtr->SetMaterial(Index, SpaceMaterialInstance.LoadSynchronous());
				}

				PrimitiveComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				PrimitiveComponentPtr->SetCollisionObjectType(Space_Object);
				PrimitiveComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				PrimitiveComponentPtr->SetRenderCustomDepth(false);

				PrimitiveComponentPtr->SetCastShadow(false);
				PrimitiveComponentPtr->bVisibleInReflectionCaptures = false;
				PrimitiveComponentPtr->bVisibleInRealTimeSkyCaptures = false;
				PrimitiveComponentPtr->bVisibleInRayTracing = false;
				PrimitiveComponentPtr->bReceivesDecals = false;
				PrimitiveComponentPtr->bUseAsOccluder = false;

				break;
			}
		}
	}

	return true;
}

bool UGT_InitializeSceneActors::ReplacedActor(
	AActor* ActorPtr
	)
{
	if (ActorPtr->IsA(ASceneElementBase::StaticClass()))
	{
		return true;
	}

	return false;
}

bool UGT_InitializeSceneActors::NormalAdjust(
	int32& Index,
	TArray<TSoftObjectPtr<ADatasmithSceneActor>>& ItemSet
	)
{
	if (RelatedActorsIndex < RelatedActors.Num())
	{
	}
	else
	{
		RelatedActorsIndex = 0;
		Index++;

		if (Index < ItemSet.Num())
		{
			ApplyRelatedActors(ItemSet[Index]);
			
			if (RelatedActorsIndex < RelatedActors.Num())
			{
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool UGT_InitializeSceneActors::NormalAdjust(
	int32& Index,
	TArray<TSoftObjectPtr<AReplaceActor>>& ItemSet
	)
{
	if (RelatedActorsIndex < RelatedActors.Num())
	{
	}
	else
	{
		RelatedActorsIndex = 0;
		Index++;

		if (Index < ItemSet.Num())
		{
			ApplyRelatedActors(ItemSet[Index]);
		}
		else
		{
			return false;
		}
	}

	return true;
}

void UGT_InitializeSceneActors::ApplyData(
	int32 Index
	)
{
	if (Index < SceneActorMap.Num())
	{
		StepIndex = 0;

		StructItemSetIndex = 0;
		StructItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].StructItemSet.DatasmithSceneActorSet)
		{
			StructItemSet.Add(Iter);
		}

		if (StructItemSet.IsValidIndex(StructItemSetIndex))
		{
			ApplyRelatedActors(StructItemSet[StructItemSetIndex]);
		}

		InnerStructItemSetIndex = 0;
		InnerStructItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].InnerStructItemSet.DatasmithSceneActorSet)
		{
			InnerStructItemSet.Add(Iter);
		}

		SoftDecorationItemSetIndex = 0;
		SoftDecorationItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].SoftDecorationItem.DatasmithSceneActorSet)
		{
			SoftDecorationItemSet.Add(Iter);
		}

		SpaceItemSetIndex = 0;
		SpaceItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].SpaceItemSet.DatasmithSceneActorSet)
		{
			SpaceItemSet.Add(Iter);
		}
	}
}

void UGT_InitializeSceneActors::ApplyRelatedActors(
	const TSoftObjectPtr<ADatasmithSceneActor>& ItemSet
	)
{
	RelatedActorsIndex = 0;
	RelatedActors.Empty();


	TArray<AActor*> OutActors;
	ItemSet->GetAttachedActors(OutActors, true, true);

	for (auto& Iter : OutActors)
	{
		if (Iter)
		{
			bool bIsSceneElement = false;
			auto Components = Iter->GetComponents();
			for (auto SecondIter : Components)
			{
				auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
				if (InterfacePtr)
				{
					auto AUDPtr = Cast<UDatasmithAssetUserData>(
					                                            InterfacePtr->GetAssetUserDataOfClass(
						                                             UDatasmithAssetUserData::StaticClass()
						                                            )
					                                           );
					if (!AUDPtr)
					{
						continue;
					}

					for (const auto& ThirdIter : UAssetRefMap::GetInstance()->NeedReplaceByUserData)
					{
						auto MetaDataIter = AUDPtr->MetaData.Find(*ThirdIter.Key.Key);
						if (MetaDataIter && (*MetaDataIter == ThirdIter.Key.Value))
						{
							auto NewActorPtr = GetWorld()->SpawnActor<ASceneElementBase>(
								 ThirdIter.Value,
								 Iter->GetActorTransform()
								);
							NewActorPtr->Replace(Iter);

							RelatedActors.Add(NewActorPtr);

							bIsSceneElement = true;
							break;
						}
					}
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
				RelatedActors.Add(Iter);
			}
		}
		else
		{
			PRINTINVOKEINFO();
		}
	}
}

void UGT_InitializeSceneActors::ApplyRelatedActors(
	const TSoftObjectPtr<AReplaceActor>& ItemSet
	)
{
	RelatedActorsIndex = 0;
	RelatedActors.Empty();

	TArray<AActor*> OutActors;
	ItemSet->GetAttachedActors(OutActors);

	for (const auto& Iter : OutActors)
	{
		RelatedActors.Add(Iter);
	}
}

UGT_SceneObjSwitch::UGT_SceneObjSwitch(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;

	Priority = FGameplayTasks::DefaultPriority;
}

void UGT_SceneObjSwitch::Activate()
{
	Super::Activate();
}

void UGT_SceneObjSwitch::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);
}

void UGT_SceneObjSwitch::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (OnEnd.IsBound())
	{
		OnEnd.Broadcast(true, Result);
	}

	Super::OnDestroy(bInOwnerFinished);
}

bool UGT_SceneObjSwitch::ProcessTask(
	float DeltaTime
	)
{
	if (FilterTags.IsEmpty())
	{
		return false;
	}

	// 要显示的DataSmith
	if (DataSmithSceneActorsSet.IsEmpty())
	{
		for (const auto& SecondIter : UAssetRefMap::GetInstance()->SceneActorMap)
		{
			if (FilterTags.Contains(SecondIter.Key))
			{
				for (const auto& ThirdIter : SecondIter.Value.StructItemSet.DatasmithSceneActorSet)
				{
					DataSmithSceneActorsSet.Add({ThirdIter, SecondIter.Value.StructItemSet});
				}
				for (const auto& ThirdIter : SecondIter.Value.StructItemSet.ReplaceActorSet)
				{
					ReplaceActorsSet.Add({ThirdIter, SecondIter.Value.StructItemSet});
				}

				for (const auto& ThirdIter : SecondIter.Value.InnerStructItemSet.DatasmithSceneActorSet)
				{
					DataSmithSceneActorsSet.Add({ThirdIter, SecondIter.Value.InnerStructItemSet});
				}
				for (const auto& ThirdIter : SecondIter.Value.InnerStructItemSet.ReplaceActorSet)
				{
					ReplaceActorsSet.Add({ThirdIter, SecondIter.Value.InnerStructItemSet});
				}

				for (const auto& ThirdIter : SecondIter.Value.SoftDecorationItem.DatasmithSceneActorSet)
				{
					DataSmithSceneActorsSet.Add({ThirdIter, SecondIter.Value.SoftDecorationItem});
				}
				for (const auto& ThirdIter : SecondIter.Value.SoftDecorationItem.ReplaceActorSet)
				{
					ReplaceActorsSet.Add({ThirdIter, SecondIter.Value.SoftDecorationItem});
				}

				for (const auto& ThirdIter : SecondIter.Value.SpaceItemSet.DatasmithSceneActorSet)
				{
					DataSmithSceneActorsSet.Add({ThirdIter, SecondIter.Value.SpaceItemSet});
				}
				for (const auto& ThirdIter : SecondIter.Value.SpaceItemSet.ReplaceActorSet)
				{
					ReplaceActorsSet.Add({ThirdIter, SecondIter.Value.SpaceItemSet});
				}
			}
		}
		if (DataSmithSceneActorsSet.IsEmpty())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	// 不显示的DataSmith
	if (HideDataSmithSceneActorsSet.IsEmpty())
	{
		for (const auto& SecondIter : UAssetRefMap::GetInstance()->AllSceneActorMap)
		{
			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempSet;

			TempSet.Append(SecondIter.StructItemSet.DatasmithSceneActorSet.Array());
			TempSet.Append(SecondIter.InnerStructItemSet.DatasmithSceneActorSet.Array());
			TempSet.Append(SecondIter.SoftDecorationItem.DatasmithSceneActorSet.Array());
			TempSet.Append(SecondIter.SpaceItemSet.DatasmithSceneActorSet.Array());

			HideDataSmithSceneActorsSet.Append(TempSet.Array());

			HideReplaceActorsSet.Append(SecondIter.StructItemSet.ReplaceActorSet.Array());
			HideReplaceActorsSet.Append(SecondIter.InnerStructItemSet.ReplaceActorSet.Array());
			HideReplaceActorsSet.Append(SecondIter.SoftDecorationItem.ReplaceActorSet.Array());
			HideReplaceActorsSet.Append(SecondIter.SpaceItemSet.ReplaceActorSet.Array());
		}
		return true;
	}

	// 不显示的
	if (HideDataSmithSceneActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			HideDataSmithSceneActorsSetIndex++;
		};
		if (HideDataSmithSceneActorsSetIndex < HideDataSmithSceneActorsSet.Num())
		{
			TArray<AActor*> OutActors;
			HideDataSmithSceneActorsSet[HideDataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors, true, true);

			for (const auto& Iter : OutActors)
			{
				auto ActorPtr = Iter;
				if (ActorPtr)
				{
					FilterCount.emplace(ActorPtr, 0);
				}
				else
				{
					PRINTINVOKEINFO();
				}
			}

			return true;
		}
	}

	if (HideReplaceActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			HideRePlaceActorsSetIndex++;
		};
		if (HideRePlaceActorsSetIndex < HideReplaceActorsSet.Num())
		{
			TArray<AActor*> RelatedActors;
			HideReplaceActorsSet[HideRePlaceActorsSetIndex]->GetAttachedActors(RelatedActors,true, true);
			for (const auto& Iter : RelatedActors)
			{
				if (Iter)
				{
					FilterCount.emplace(Iter, 0);
				}
				else
				{
					PRINTINVOKEINFO();
				}
			}
			return true;
		}
	}

	// 确认过滤条件数量
	if (DataSmithSceneActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			DataSmithSceneActorsSetIndex++;
		};
		if (DataSmithSceneActorsSetIndex < DataSmithSceneActorsSet.Num())
		{
			TArray<AActor*> OutActors;
			DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex].Key->GetAttachedActors(OutActors, true, true);

			for (const auto& Iter : OutActors)
			{
				auto ActorPtr = Iter;
				if (ActorPtr)
				{
					bool bIsOK = true;

					if (DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex].Value.TypeSet.IsEmpty())
					{
					}
					else
					{
						bIsOK = false;
						for (const auto& FilterIter : DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex].Value.
						     TypeSet)
						{
							if (ActorPtr->IsA(FilterIter))
							{
								bIsOK = true;
								break;
							}
						}
					}

					if (bIsOK)
					{
						FilterCount[Iter] = 1;
					}
				}
				else
				{
					PRINTINVOKEINFO();
				}
			}

			return true;
		}
		else
		{
		}
	}

	if (ReplaceActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			ReplaceActorsSetIndex++;
		};
		if (ReplaceActorsSetIndex < ReplaceActorsSet.Num())
		{
			TArray<AActor*> RelatedActors;
			ReplaceActorsSet[ReplaceActorsSetIndex].Key->GetAttachedActors(RelatedActors,true, true);
			for (const auto& Iter : RelatedActors)
			{
				if (Iter)
				{
					FilterCount[Iter] = 1;
				}
				else
				{
					PRINTINVOKEINFO();
				}
			}
			return true;
		}
		else
		{
		}
	}

	// 显示
	ON_SCOPE_EXIT
	{
		FilterIndex++;
	};
	if (FilterIndex < FilterCount.size())
	{
		auto Iter = FilterCount.begin();

		std::advance(Iter, FilterIndex);
		if (Iter->second > 0)
		{
			if (Iter->first->GetActorLabel() == TEXT("机械设备_风机盘管-卧式暗装风机盘管_带下回风箱50Pa_-FP-34_238WA-Y3-G50／BXH_FP-102WA-Y3-G50_BXH_62"))
			{
					PRINTINVOKEINFO();
			}
		
			Iter->first->SetActorHiddenInGame(false);
			Result.Add(Iter->first);
		}
		else
		{
			Iter->first->SetActorHiddenInGame(true);
		}
		return true;
	}
	else
	{
		return false;
	}
}

UGT_FloorSplit::UGT_FloorSplit(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;

	Priority = 1.5 * FGameplayTasks::DefaultPriority;
}

void UGT_FloorSplit::Activate()
{
	Super::Activate();
}

void UGT_FloorSplit::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);
}

void UGT_FloorSplit::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (OnEnd.IsBound())
	{
		OnEnd.Broadcast(true);
	}

	Super::OnDestroy(bInOwnerFinished);
}

bool UGT_FloorSplit::ProcessTask(
	float DeltaTime
	)
{
	switch (StepIndex)
	{
	case 0:
		{
			if (ProcessTask_Sort())
			{
				return true;
			}
			else
			{
				StepIndex++;
				return true;
			}
		}
	case 1:
		{
			if (ProcessTask_Display())
			{
				return true;
			}
			else
			{
				StepIndex++;

				UseScopeType = EUseScopeType::kNone;

				return true;
			}
		}
	case 2:
		{
			if (ProcessTask_Move(DeltaTime))
			{
				return true;
			}
			else
			{
				StepIndex++;
				return true;
			}
		}
	}

	return false;
}

bool UGT_FloorSplit::ProcessTask_Sort()
{
	// 要显示的DataSmith
	if (DataSmithSceneActorsSet.IsEmpty())
	{
		for (const auto& SecondIter : UAssetRefMap::GetInstance()->SceneActorMap)
		{
			//
			bool bIsFloorData = false;

			FGameplayTagContainer GameplayTagContainer;

			GameplayTagContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
			GameplayTagContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_SplitFloor);

			if (SecondIter.Key.ConditionalSet.HasAll(GameplayTagContainer))
			{
				for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
				{
					if (SecondIter.Key.ConditionalSet.HasTag(FloorIter.Value->FloorTag))
					{
						bIsFloorData = true;

						TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempSet;

						TempSet.Append(SecondIter.Value.StructItemSet.DatasmithSceneActorSet.Array());
						TempSet.Append(SecondIter.Value.InnerStructItemSet.DatasmithSceneActorSet.Array());
						TempSet.Append(SecondIter.Value.SoftDecorationItem.DatasmithSceneActorSet.Array());
						TempSet.Append(SecondIter.Value.SpaceItemSet.DatasmithSceneActorSet.Array());

						DataSmithSceneActorsSet.Add(FloorIter.Key, TempSet.Array());

						ReplaceActorsSet.Add(FloorIter.Key, SecondIter.Value.StructItemSet.ReplaceActorSet.Array());
						ReplaceActorsSet.Add(
						                     FloorIter.Key,
						                     SecondIter.Value.InnerStructItemSet.ReplaceActorSet.Array()
						                    );
						ReplaceActorsSet.Add(
						                     FloorIter.Key,
						                     SecondIter.Value.SoftDecorationItem.ReplaceActorSet.Array()
						                    );
						ReplaceActorsSet.Add(FloorIter.Key, SecondIter.Value.SpaceItemSet.ReplaceActorSet.Array());

						break;
					}
				}
			}

			if (bIsFloorData)
			{
			}
			else
			{
				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempSet;

				TempSet.Append(SecondIter.Value.StructItemSet.DatasmithSceneActorSet.Array());
				TempSet.Append(SecondIter.Value.InnerStructItemSet.DatasmithSceneActorSet.Array());
				TempSet.Append(SecondIter.Value.SoftDecorationItem.DatasmithSceneActorSet.Array());
				TempSet.Append(SecondIter.Value.SpaceItemSet.DatasmithSceneActorSet.Array());

				HideDataSmithSceneActorsSet.Append(TempSet.Array());

				HideReplaceActorsSet.Append(SecondIter.Value.StructItemSet.ReplaceActorSet.Array());
				HideReplaceActorsSet.Append(SecondIter.Value.InnerStructItemSet.ReplaceActorSet.Array());
				HideReplaceActorsSet.Append(SecondIter.Value.SoftDecorationItem.ReplaceActorSet.Array());
				HideReplaceActorsSet.Append(SecondIter.Value.SpaceItemSet.ReplaceActorSet.Array());
			}
		}
	}

	return false;
}

bool UGT_FloorSplit::ProcessTask_Display()
{
	// 不显示的
	if (HideDataSmithSceneActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			HideDataSmithSceneActorsSetIndex++;
		};
		if (HideDataSmithSceneActorsSetIndex < HideDataSmithSceneActorsSet.Num())
		{
			TArray<AActor*> OutActors;
			HideDataSmithSceneActorsSet[HideDataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors, true, true);

			for (const auto& Iter : OutActors)
			{
				auto ActorPtr = Iter;
				if (ActorPtr)
				{
					FilterCount.emplace(ActorPtr, 0);
				}
				else
				{
					PRINTINVOKEINFO();
				}
			}

			return true;
		}
	}
	if (HideReplaceActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			HideRePlaceActorsSetIndex++;
		};
		if (HideRePlaceActorsSetIndex < HideReplaceActorsSet.Num())
		{
			TArray<AActor*> RelatedActors;
			HideReplaceActorsSet[HideRePlaceActorsSetIndex]->GetAttachedActors(RelatedActors,true, true);
			for (const auto& Iter : RelatedActors)
			{
				if (Iter)
				{
					FilterCount.emplace(Iter, 0);
				}
				else
				{
					PRINTINVOKEINFO();
				}
			}
			return true;
		}
	}

	// 确认过滤条件数量
	if (DataSmithSceneActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			DataSmithSceneActorsSetIndex++;
		};
		if (DataSmithSceneActorsSet.Contains(DataSmithSceneActorsSetIndex))
		{
			for (const auto& Iter : DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex])
			{
				TArray<AActor*> OutActors;
				Iter->GetAttachedActors(OutActors, true, true);

				for (const auto& SecondIter : OutActors)
				{
					auto ActorPtr = SecondIter;
					if (ActorPtr)
					{
						FilterCount[ActorPtr] = 1;
					}
					else
					{
						PRINTINVOKEINFO();
					}
				}
			}

			return true;
		}
		else
		{
		}
	}
	if (ReplaceActorsSet.IsEmpty())
	{
	}
	else
	{
		ON_SCOPE_EXIT
		{
			ReplaceActorsSetIndex++;
		};
		if (ReplaceActorsSet.Contains(ReplaceActorsSetIndex))
		{
			for (const auto& Iter : ReplaceActorsSet[ReplaceActorsSetIndex])
			{
				TArray<AActor*> RelatedActors;
				Iter->GetAttachedActors(RelatedActors,true, true);
				for (const auto& SecondIter : RelatedActors)
				{
					if (SecondIter)
					{
						FilterCount[SecondIter] = 1;
					}
					else
					{
						PRINTINVOKEINFO();
					}
				}
			}
			return true;
		}
		else
		{
		}
	}

	// 显示
	ON_SCOPE_EXIT
	{
		FilterIndex++;
	};
	if (FilterIndex < FilterCount.size())
	{
		auto Iter = FilterCount.begin();
		std::advance(Iter, FilterIndex);
		if (Iter->second > 0)
		{
			Iter->first->SetActorHiddenInGame(false);
		}
		else
		{
			Iter->first->SetActorHiddenInGame(true);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool UGT_FloorSplit::ProcessTask_Move(
	float DeltaTime
	)
{
	bool bIsEnd = false;

	ConsumeTime += DeltaTime;
	if (ConsumeTime > MoveDuration)
	{
		bIsEnd = true;
		ConsumeTime = MoveDuration;
	}

	const auto Percent = ConsumeTime / MoveDuration;

	FVector Offset = FVector::ZeroVector;

	for (const auto& Iter : DataSmithSceneActorsSet)
	{
		Offset = FVector(0, 0, Iter.Key * HeightInterval);

		for (const auto& SecondIter : Iter.Value)
		{
			SecondIter->SetActorLocation((Percent * Offset));
		}
	}

	return !bIsEnd;
}

UGT_QuitFloorSplit::UGT_QuitFloorSplit(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;

	Priority = 1.5 * FGameplayTasks::DefaultPriority;
}

void UGT_QuitFloorSplit::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (OnEnd.IsBound())
	{
		OnEnd.Broadcast(true);
	}

	Super::OnDestroy(bInOwnerFinished);
}

bool UGT_QuitFloorSplit::ProcessTask(
	float DeltaTime
	)
{
	switch (StepIndex)
	{
	case 0:
		{
			if (ProcessTask_Sort())
			{
				return true;
			}
			else
			{
				StepIndex++;
				return true;
			}
		}
	case 1:
		{
			if (ProcessTask_Move(DeltaTime))
			{
				return true;
			}
			else
			{
				StepIndex++;
				return true;
			}
		}
	}

	return false;
}

bool UGT_QuitFloorSplit::ProcessTask_Sort()
{
	// 要显示的DataSmith
	if (DataSmithSceneActorsSet.IsEmpty())
	{
		for (const auto& SecondIter : UAssetRefMap::GetInstance()->SceneActorMap)
		{
			//
			bool bIsFloorData = false;

			FGameplayTagContainer GameplayTagContainer;

			GameplayTagContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
			GameplayTagContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_SplitFloor);

			if (SecondIter.Key.ConditionalSet.HasAll(GameplayTagContainer))
			{
				for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
				{
					if (SecondIter.Key.ConditionalSet.HasTag(FloorIter.Value->FloorTag))
					{
						bIsFloorData = true;

						TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempSet;

						TempSet.Append(SecondIter.Value.StructItemSet.DatasmithSceneActorSet.Array());
						TempSet.Append(SecondIter.Value.InnerStructItemSet.DatasmithSceneActorSet.Array());
						TempSet.Append(SecondIter.Value.SoftDecorationItem.DatasmithSceneActorSet.Array());
						TempSet.Append(SecondIter.Value.SpaceItemSet.DatasmithSceneActorSet.Array());

						DataSmithSceneActorsSet.Add(FloorIter.Key, TempSet.Array());

						ReplaceActorsSet.Add(FloorIter.Key, SecondIter.Value.StructItemSet.ReplaceActorSet.Array());
						ReplaceActorsSet.Add(
						                     FloorIter.Key,
						                     SecondIter.Value.InnerStructItemSet.ReplaceActorSet.Array()
						                    );
						ReplaceActorsSet.Add(
						                     FloorIter.Key,
						                     SecondIter.Value.SoftDecorationItem.ReplaceActorSet.Array()
						                    );
						ReplaceActorsSet.Add(FloorIter.Key, SecondIter.Value.SpaceItemSet.ReplaceActorSet.Array());

						break;
					}
				}
			}
		}
	}

	return false;
}

bool UGT_QuitFloorSplit::ProcessTask_Move(
	float DeltaTime
	)
{
	bool bIsEnd = false;

	ConsumeTime += DeltaTime;
	if (ConsumeTime > MoveDuration)
	{
		bIsEnd = true;
		ConsumeTime = MoveDuration;
	}

	const auto Percent = 1 - (ConsumeTime / MoveDuration);

	FVector Offset = FVector::ZeroVector;

	for (const auto& Iter : DataSmithSceneActorsSet)
	{
		Offset = FVector(0, 0, Iter.Key * HeightInterval);

		for (const auto& SecondIter : Iter.Value)
		{
			SecondIter->SetActorLocation((Percent * Offset));
		}
	}

	return !bIsEnd;
}
