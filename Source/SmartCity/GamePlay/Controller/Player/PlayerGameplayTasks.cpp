#include "PlayerGameplayTasks.h"

#include "CollisionDataStruct.h"
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
#include "GenerateTypes.h"
#include "LogWriter.h"
#include "ReplaceActor.h"
#include "SceneElementBase.h"
#include "TemplateHelper.h"

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

	if (bUseScope)
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

			if (ProcessTask())
			{
			}
			else
			{
				break;
			}
		}
	}

	if (bUseScope)
	{
	}
	else
	{
		for (;;)
		{
			if (ProcessTask())
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

	EndTask();
}

bool UGT_BatchBase::ProcessTask()
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

	bUseScope = false;

	PerTickProcessNum = 1000;
}

void UGT_InitializeSceneActors::Activate()
{
	Super::Activate();

	for (const auto& Iter : UAssetRefMap::GetInstance()->SceneActorMap)
	{
		SceneActorMap.Add(Iter.Value);
	}

	ApplyData(0);
	SetIndex = -1;
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

bool UGT_InitializeSceneActors::ProcessTask()
{
	if (SceneActorMapIndex < SceneActorMap.Num())
	{
	}
	else
	{
		return false;
	}

	switch (SetIndex)
	{
	case -1:
		{
			if (ProcessTask_NeedReplaceByRef())
			{
				return true;
			}
			else
			{
				SetIndex++;
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

				SetIndex++;
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

				SetIndex++;
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
				if (ReplaceSoftDecorationItemSetIndex < ReplaceSoftDecorationItemSet.Num())
				{
					ApplyRelatedActors(ReplaceSoftDecorationItemSet[ReplaceSoftDecorationItemSetIndex]);
				}
				else
				{
					RelatedActorsIndex = 0;
					RelatedActors.Empty();
				}

				SetIndex++;
				return true;
			}
		}
	case 3:
		{
			if (ProcessTask_ReplaceSoftDecorationItemSet())
			{
				return true;
			}
			else
			{
				if (SpaceItemSetIndex < SpaceItemSet.Num())
				{
					ApplyRelatedActors(SpaceItemSet[SpaceItemSetIndex]);
				}
				else
				{
					RelatedActorsIndex = 0;
					RelatedActors.Empty();
				}

				SetIndex++;
				return true;
			}
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

bool UGT_InitializeSceneActors::ProcessTask_NeedReplaceByRef()
{
	auto NeedReplaceByRef = UAssetRefMap::GetInstance()->NeedReplaceByRef;

	for (auto Iter : NeedReplaceByRef)
	{
		TArray<AActor*> OutActors;
		Iter.Key->GetAttachedActors(OutActors, true, true);
		
		for (auto & SecondIter : OutActors)
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
	if (NormalAdjust(ReplaceSoftDecorationItemSetIndex, ReplaceSoftDecorationItemSet))
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
				PrimitiveComponentPtr->SetCollisionObjectType(Device_Object);
				PrimitiveComponentPtr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

				PrimitiveComponentPtr->SetRenderCustomDepth(false);

				break;
			}
		}
	}

	return true;
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
		SetIndex = 0;

		StructItemSetIndex = 0;
		StructItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].StructItemSet)
		{
			StructItemSet.Add(Iter);
		}

		if (StructItemSet.IsValidIndex(StructItemSetIndex))
		{
			ApplyRelatedActors(StructItemSet[StructItemSetIndex]);
		}

		InnerStructItemSetIndex = 0;
		InnerStructItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].InnerStructItemSet)
		{
			InnerStructItemSet.Add(Iter);
		}

		SoftDecorationItemSetIndex = 0;
		SoftDecorationItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].SoftDecorationItemSet)
		{
			SoftDecorationItemSet.Add(Iter);
		}

		ReplaceSoftDecorationItemSetIndex = 0;
		ReplaceSoftDecorationItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].ReplaceSoftDecorationItemSet)
		{
			ReplaceSoftDecorationItemSet.Add(Iter);
		}

		SpaceItemSetIndex = 0;
		SpaceItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].SpaceItemSet)
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

							RelatedActors.Add( NewActorPtr);

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
	OnEnd.Broadcast(true, Result);

	Super::OnDestroy(bInOwnerFinished);
}

bool UGT_SceneObjSwitch::ProcessTask()
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
				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempSet;
				
				TempSet.Append(SecondIter.Value.StructItemSet.Array());
				TempSet.Append(SecondIter.Value.InnerStructItemSet.Array());
				TempSet.Append(SecondIter.Value.SoftDecorationItemSet.Array());
				TempSet.Append(SecondIter.Value.SpaceItemSet.Array());
				
				ReplaceActorsSet.Append(SecondIter.Value.ReplaceSoftDecorationItemSet.Array());
				DataSmithSceneActorsSet.Append(TempSet.Array());
			}
			else
			{
				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempSet;
				
				TempSet.Append(SecondIter.Value.StructItemSet.Array());
				TempSet.Append(SecondIter.Value.InnerStructItemSet.Array());
				TempSet.Append(SecondIter.Value.SoftDecorationItemSet.Array());
				TempSet.Append(SecondIter.Value.SpaceItemSet.Array());
				
				HideReplaceActorsSet.Append(SecondIter.Value.ReplaceSoftDecorationItemSet.Array());
				HideDataSmithSceneActorsSet.Append(TempSet.Array());
			}
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

			// TArray<AActor*> OutActors;
			// HideDataSmithSceneActorsSet[HideDataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors);
			// for (const auto& Iter : OutActors)
			// {
			// 	if (Iter)
			// 	{
			// 		FilterCount.emplace(Iter, 0);
			// 	}
			// 	else
			// 	{
			// 		PRINTINVOKEINFO();
			// 	}
			// }
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
			HideReplaceActorsSet[HideRePlaceActorsSetIndex]->GetAttachedActors(RelatedActors);
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
			DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors, true, true);
		
			for (const auto& Iter : OutActors)
			{
				auto ActorPtr = Iter;
				if (ActorPtr)
				{
					FilterCount[Iter] = 1;
				}
				else
				{
					PRINTINVOKEINFO();
				}
			}
			
			// TArray<AActor*> OutActors;
			// DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors);
			// for (const auto& Iter : OutActors)
			// {
			// 	if (Iter)
			// 	{
			// 		FilterCount[Iter] = 1;
			// 	}
			// 	else
			// 	{
			// 		PRINTINVOKEINFO();
			// 	}
			// }
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
			ReplaceActorsSet[ReplaceActorsSetIndex]->GetAttachedActors(RelatedActors);
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
