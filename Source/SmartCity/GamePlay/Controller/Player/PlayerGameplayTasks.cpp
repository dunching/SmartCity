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

	auto Result = UKismetAlgorithm::GetCameraSeat(
	                                              {TargetDevicePtr.Get()},
	                                              UGameOptions::GetInstance()->ViewDeviceRot,
	                                              UGameOptions::GetInstance()->ViewDeviceControlParam.FOV
	                                             );

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
		SceneActorMap.Add(Iter.Value->AllReference);
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->BuildingHelpers)
	{
		SceneActorMap.Add(Iter.Value->AllReference);
	}

	ApplyData(0);

	Step = EStep::kRecordFloor;
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

	switch (Step)
	{
	case EStep::kRecordFloor:
		{
			if (ProcessTask_RecordFloor())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kNeedReplaceByRef;
			return true;
		}
	case EStep::kNeedReplaceByRef:
		{
			if (ProcessTask_NeedReplaceByRef())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kStructItemSet;
			return true;
		}
	case EStep::kStructItemSet:
		{
			if (ProcessTask_StructItemSet())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kInnerStructItemSet;
			return true;
		}
	case EStep::kInnerStructItemSet:
		{
			if (ProcessTask_InnerStructItemSet())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kSoftDecorationItemSet;
			return true;
		}
	case EStep::kSoftDecorationItemSet:
		{
			if (ProcessTask_SoftDecorationItemSet())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kSpaceItemSet;
			return true;
		}
	case EStep::kSpaceItemSet:
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
			NewActorPtr->Replace(SecondIter, {}, {});
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

	ApplyRelatedActors(StructItemSet[StructItemSetIndex]);

	ON_SCOPE_EXIT
	{
		StructItemSetIndex++;
	};

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

	ApplyRelatedActors(InnerStructItemSet[InnerStructItemSetIndex]);

	ON_SCOPE_EXIT
	{
		InnerStructItemSetIndex++;
	};

	return true;
}

bool UGT_InitializeSceneActors::ProcessTask_SoftDecorationItemSet()
{
	if (SoftDecorationItemSetIndex < SoftDecorationItemSet.Num())
	{
	}
	else
	{
		return false;
	}

	ApplyRelatedActors(SoftDecorationItemSet[SoftDecorationItemSetIndex]);

	ON_SCOPE_EXIT
	{
		SoftDecorationItemSetIndex++;
	};

	return true;
}

bool UGT_InitializeSceneActors::ProcessTask_ReplaceSoftDecorationItemSet()
{
	return false;
}

bool UGT_InitializeSceneActors::ProcessTask_SpaceItemSet()
{
	if (SpaceItemSetIndex < SpaceItemSet.Num())
	{
	}
	else
	{
		return false;
	}

	ON_SCOPE_EXIT
	{
		SpaceItemSetIndex++;
	};

	const auto SpaceInfo = UAssetRefMap::GetInstance()->SpaceInfo;

	TArray<AActor*> OutActors;
	SpaceItemSet[SpaceItemSetIndex]->GetAttachedActors(OutActors, true, true);

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

void UGT_InitializeSceneActors::ApplyData(
	int32 Index
	)
{
	if (Index < SceneActorMap.Num())
	{
		Step = EStep::kStructItemSet;

		StructItemSetIndex = 0;
		StructItemSet.Empty();
		for (const auto& Iter : SceneActorMap[Index].StructItemSet.DatasmithSceneActorSet)
		{
			StructItemSet.Add(Iter);
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
	ItemSet->GetRootComponent()->SetMobility(EComponentMobility::Movable);

	TArray<AActor*> OutActors;
	ItemSet->GetAttachedActors(OutActors, true, true);

	TMap<int32, ASceneElementBase*> MergeActorsMap;
	for (auto& Iter : OutActors)
	{
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
					NewActorPtr->Replace(Iter, {*ThirdIter.Key.Key, *MetaDataIter},AUDPtr->MetaData);
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
					NewActorPtr->SceneElementID = *Datasmith_UniqueId;

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
				NewActorPtr->Replace(Iter, {},{});
				NewActorPtr->InitialSceneElement();
			}
			else
			{
				continue;
			}
		}
	}
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
		OnEnd.Broadcast(true, Result, this);
	}

	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_Display()
{
	if (FilterTags.ConditionalSet.IsEmpty())
	{
		return false;
	}

	if (FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
	    FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_SplitFloor))
	{
		auto Lambda = [&](
			const FSceneElementMap& AllReference
			)
		{
			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

			TempDataSmithSceneActorsSet.Append(
			                                   AllReference.StructItemSet.DatasmithSceneActorSet.
			                                                Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   AllReference.InnerStructItemSet.DatasmithSceneActorSet.
			                                                Array()
			                                  );

			DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

			ReplaceActorsSet.Append(
			                        AllReference.StructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        AllReference.InnerStructItemSet.OtherItem
			                       );

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       AllReference.SoftDecorationItem.
			                                                    DatasmithSceneActorSet.
			                                                    Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       AllReference.SpaceItemSet.DatasmithSceneActorSet.
			                                                    Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(AllReference.SoftDecorationItem.OtherItem);
			HideReplaceActorsSet.Append(AllReference.SpaceItemSet.OtherItem);
		};
		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			FloorIter.Value->SwitchInteractionType(FilterTags);
			Lambda(FloorIter.Value->AllReference);
		}
		for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
		{
			Lambda(Iter.Value->AllReference);
		}

		return false;
	}

	if (FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space))
	{
		auto FloorTag = USmartCitySuiteTags::Interaction_Area_Floor.GetTag();
		for (const auto Iter : FilterTags.ConditionalSet)
		{
			if (Iter.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
			{
				FloorTag = Iter;
				break;
			}
		}

		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
			{
				FloorIter.Value->SwitchInteractionType(FilterTags);

				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

				const auto FloorIndex = FloorIter.Value->FloorIndex;

				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
				                                             Array()
				                                  );
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.InnerStructItemSet.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.SoftDecorationItem.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.SpaceItemSet.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );

				DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.StructItemSet.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.SpaceItemSet.OtherItem
				                       );
			}
			else
			{
				FloorIter.Value->SwitchInteractionType(FSceneElementConditional::EmptyConditional);

				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.StructItemSet.
				                                                 DatasmithSceneActorSet.
				                                                 Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.InnerStructItemSet.
				                                                 DatasmithSceneActorSet
				                                                 .Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.SoftDecorationItem.
				                                                 DatasmithSceneActorSet.
				                                                 Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
				                                                .Array()
				                                      );

				HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.StructItemSet.OtherItem
				                           );
				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
				                           );
				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
				                           );
				HideReplaceActorsSet.Append(FloorIter.Value->AllReference.SpaceItemSet.OtherItem);
			}
		}

		for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
		{
			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.StructItemSet.
			                                            DatasmithSceneActorSet.
			                                            Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.InnerStructItemSet.
			                                            DatasmithSceneActorSet
			                                            .Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.SoftDecorationItem.
			                                            DatasmithSceneActorSet.
			                                            Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
			                                           .Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.StructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.InnerStructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.SoftDecorationItem.OtherItem
			                           );
			HideReplaceActorsSet.Append(Iter.Value->AllReference.SpaceItemSet.OtherItem);
		}

		return false;
	}

	if (FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor))
	{
		auto FloorTag = USmartCitySuiteTags::Interaction_Area_Floor.GetTag();
		for (const auto Iter : FilterTags.ConditionalSet)
		{
			if (Iter.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
			{
				FloorTag = Iter;
				break;
			}
		}

		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
			{
				FloorIter.Value->SwitchInteractionType(FilterTags);

				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

				const auto FloorIndex = FloorIter.Value->FloorIndex;

				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
				                                             Array()
				                                  );
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.InnerStructItemSet.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.SoftDecorationItem.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.SpaceItemSet.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );

				DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.StructItemSet.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.SpaceItemSet.OtherItem
				                       );
			}
			else
			{
				FloorIter.Value->SwitchInteractionType(FSceneElementConditional::EmptyConditional);

				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.StructItemSet.
				                                                 DatasmithSceneActorSet.
				                                                 Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.InnerStructItemSet.
				                                                 DatasmithSceneActorSet
				                                                 .Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.SoftDecorationItem.
				                                                 DatasmithSceneActorSet.
				                                                 Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
				                                                .Array()
				                                      );

				HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.StructItemSet.OtherItem
				                           );
				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
				                           );
				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
				                           );
				HideReplaceActorsSet.Append(FloorIter.Value->AllReference.SpaceItemSet.OtherItem);
			}
		}
		for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
		{
			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.StructItemSet.
			                                            DatasmithSceneActorSet.
			                                            Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.InnerStructItemSet.
			                                            DatasmithSceneActorSet
			                                            .Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.SoftDecorationItem.
			                                            DatasmithSceneActorSet.
			                                            Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
			                                           .Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.StructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.InnerStructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.SoftDecorationItem.OtherItem
			                           );
			HideReplaceActorsSet.Append(Iter.Value->AllReference.SpaceItemSet.OtherItem);
		}

		return false;
	}

	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_Hiden()
{
	if (DataSmithSceneActorsSetIndex < DataSmithSceneActorsSet.Num())
	{
		ON_SCOPE_EXIT
		{
			DataSmithSceneActorsSetIndex++;
		};

		TArray<AActor*> OutActors;
		DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors, true, true);

		DisplayAry.Append(OutActors);

		return true;
	}

	if (!ReplaceActorsSet.IsEmpty())
	{
		for (auto Iter : ReplaceActorsSet)
		{
			DisplayAry.Add(Iter.LoadSynchronous());
		}

		ReplaceActorsSet.Empty();

		return true;
	}

	if (HideDataSmithSceneActorsSetIndex < HideDataSmithSceneActorsSet.Num())
	{
		ON_SCOPE_EXIT
		{
			HideDataSmithSceneActorsSetIndex++;
		};

		TArray<AActor*> OutActors;
		HideDataSmithSceneActorsSet[HideDataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors, true, true);

		HideAry.Append(OutActors);

		return true;
	}

	if (!HideReplaceActorsSet.IsEmpty())
	{
		for (auto Iter : HideReplaceActorsSet)
		{
			HideAry.Add(Iter.LoadSynchronous());
		}

		HideReplaceActorsSet.Empty();

		return true;
	}

	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_ConfirmConditional()
{
	return false;
}

bool UGT_SwitchSceneElement_Base::ProcessTask_SwitchState()
{
	if (DisplayAryIndex < DisplayAry.Num())
	{
		ON_SCOPE_EXIT
		{
			DisplayAryIndex++;
		};

		auto ActorPtr = DisplayAry[DisplayAryIndex];
		if (ActorPtr)
		{
			auto SceneElementPtr = Cast<ASceneElementBase>(ActorPtr);
			if (SceneElementPtr)
			{
				SceneElementPtr->SwitchInteractionType(FilterTags);
				Result.Add(SceneElementPtr);
			}
			else
			{
				ActorPtr->SetActorHiddenInGame(false);
				Result.Add(SceneElementPtr);
			}
		}

		return true;
	}

	if (HideAryIndex < HideAry.Num())
	{
		ON_SCOPE_EXIT
		{
			HideAryIndex++;
		};

		auto ActorPtr = HideAry[HideAryIndex];
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

void UGT_SwitchSceneElement_Generic::Activate()
{
	Super::Activate();
}

bool UGT_SwitchSceneElement_Generic::ProcessTask_Display()
{
	if (FilterTags.ConditionalSet.IsEmpty())
	{
		return false;
	}

	if (
		FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
		FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Periphery) ||
	    FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_SplitFloor)
	    )
	{
		auto Lambda = [&](
			const FSceneElementMap& AllReference
			)
		{
			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

			TempDataSmithSceneActorsSet.Append(
			                                   AllReference.StructItemSet.DatasmithSceneActorSet.
			                                                Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   AllReference.InnerStructItemSet.DatasmithSceneActorSet.
			                                                Array()
			                                  );

			DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

			ReplaceActorsSet.Append(
			                        AllReference.StructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        AllReference.InnerStructItemSet.OtherItem
			                       );

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       AllReference.SoftDecorationItem.
			                                                    DatasmithSceneActorSet.
			                                                    Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       AllReference.SpaceItemSet.DatasmithSceneActorSet.
			                                                    Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(AllReference.SoftDecorationItem.OtherItem);
			HideReplaceActorsSet.Append(AllReference.SpaceItemSet.OtherItem);
		};
		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			FloorIter.Value->SwitchInteractionType(FilterTags);
			Lambda(FloorIter.Value->AllReference);
		}
		for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
		{
			Iter.Value->SwitchInteractionType(FilterTags);

			Lambda(Iter.Value->AllReference);
		}

		return false;
	}

	if (FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor))
	{
		auto FloorTag = USmartCitySuiteTags::Interaction_Area_Floor.GetTag();
		for (const auto Iter : FilterTags.ConditionalSet)
		{
			if (Iter.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
			{
				FloorTag = Iter;
				break;
			}
		}

		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
			{
				FloorIter.Value->SwitchInteractionType(FilterTags);

				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

				const auto FloorIndex = FloorIter.Value->FloorIndex;

				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
				                                             Array()
				                                  );
				
				if (bDisplayInnerStruct)
				{
					DataSmithSceneActorsSet.Append(FloorIter.Value->AllReference.InnerStructItemSet.
															 DatasmithSceneActorSet.
															 Array());
				}
				else
				{
					HideDataSmithSceneActorsSet.Append(FloorIter.Value->AllReference.InnerStructItemSet.
															 DatasmithSceneActorSet.
															 Array());
				}
				
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.SoftDecorationItem.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );
				TempDataSmithSceneActorsSet.Append(
				                                   FloorIter.Value->AllReference.SpaceItemSet.
				                                             DatasmithSceneActorSet.
				                                             Array()
				                                  );

				DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.StructItemSet.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
				                       );
				ReplaceActorsSet.Append(
				                        FloorIter.Value->AllReference.SpaceItemSet.OtherItem
				                       );
			}
			else
			{
				FloorIter.Value->SwitchInteractionType(FSceneElementConditional::EmptyConditional);

				TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.StructItemSet.
				                                                 DatasmithSceneActorSet.
				                                                 Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.InnerStructItemSet.
				                                                 DatasmithSceneActorSet
				                                                 .Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.SoftDecorationItem.
				                                                 DatasmithSceneActorSet.
				                                                 Array()
				                                      );
				TempHideDataSmithSceneActorsSet.Append(
				                                       FloorIter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
				                                                .Array()
				                                      );

				HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.StructItemSet.OtherItem
				                           );
				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
				                           );
				HideReplaceActorsSet.Append(
				                            FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
				                           );
				HideReplaceActorsSet.Append(FloorIter.Value->AllReference.SpaceItemSet.OtherItem);
			}
		}
		for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
		{
			Iter.Value->SwitchInteractionType(FilterTags);

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.StructItemSet.
			                                            DatasmithSceneActorSet.
			                                            Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.InnerStructItemSet.
			                                            DatasmithSceneActorSet
			                                            .Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.SoftDecorationItem.
			                                            DatasmithSceneActorSet.
			                                            Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       Iter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
			                                           .Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.StructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.InnerStructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            Iter.Value->AllReference.SoftDecorationItem.OtherItem
			                           );
			HideReplaceActorsSet.Append(Iter.Value->AllReference.SpaceItemSet.OtherItem);
		}

		return false;
	}

	return false;
}

bool UGT_SwitchSceneElement_Generic::ProcessTask_Hiden()
{
	if (DataSmithSceneActorsSetIndex < DataSmithSceneActorsSet.Num())
	{
		ON_SCOPE_EXIT
		{
			DataSmithSceneActorsSetIndex++;
		};

		TArray<AActor*> OutActors;
		DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors, true, true);

		DisplayAry.Append(OutActors);

		return true;
	}

	if (!ReplaceActorsSet.IsEmpty())
	{
		for (auto Iter : ReplaceActorsSet)
		{
			DisplayAry.Add(Iter.LoadSynchronous());
		}

		ReplaceActorsSet.Empty();

		return true;
	}

	if (HideDataSmithSceneActorsSetIndex < HideDataSmithSceneActorsSet.Num())
	{
		ON_SCOPE_EXIT
		{
			HideDataSmithSceneActorsSetIndex++;
		};

		TArray<AActor*> OutActors;
		HideDataSmithSceneActorsSet[HideDataSmithSceneActorsSetIndex]->GetAttachedActors(OutActors, true, true);

		HideAry.Append(OutActors);

		return true;
	}

	if (!HideReplaceActorsSet.IsEmpty())
	{
		for (auto Iter : HideReplaceActorsSet)
		{
			HideAry.Add(Iter.LoadSynchronous());
		}

		HideReplaceActorsSet.Empty();

		return true;
	}

	return false;
}

bool UGT_SwitchSceneElement_Generic::ProcessTask_ConfirmConditional()
{
	return false;
}

bool UGT_SwitchSceneElement_Generic::ProcessTask_SwitchState()
{
	if (DisplayAryIndex < DisplayAry.Num())
	{
		ON_SCOPE_EXIT
		{
			DisplayAryIndex++;
		};

		auto ActorPtr = DisplayAry[DisplayAryIndex];
		if (ActorPtr)
		{
			auto SceneElementPtr = Cast<ASceneElementBase>(ActorPtr);
			if (SceneElementPtr)
			{
				SceneElementPtr->SwitchInteractionType(FilterTags);
				Result.Add(SceneElementPtr);
			}
			else
			{
				ActorPtr->SetActorHiddenInGame(false);
				Result.Add(SceneElementPtr);
			}
		}

		return true;
	}

	if (HideAryIndex < HideAry.Num())
	{
		ON_SCOPE_EXIT
		{
			HideAryIndex++;
		};

		auto ActorPtr = HideAry[HideAryIndex];
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
			}
		}

		return true;
	}

	return false;
}

bool UGT_SwitchSceneElement_Generic::ProcessTask_SwitchState_Elevator()
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

void UGT_SwitchSceneElement_Space::Activate()
{
	Super::Activate();

	if (SceneElementPtr.IsValid())
	{
		Floor = SceneElementPtr->BelongFloor->FloorTag;
	}

	FilterTags.ConditionalSet = FGameplayTagContainer{Floor};
}

bool UGT_SwitchSceneElement_Space::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(Floor))
		{
			FloorIter.Value->SwitchInteractionType(FilterTags);

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			const auto FloorIndex = FloorIter.Value->FloorIndex;

			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempHideDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.InnerStructItemSet.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SoftDecorationItem.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SpaceItemSet.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.StructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.SpaceItemSet.OtherItem
			                       );
		}
		else
		{
			FloorIter.Value->SwitchInteractionType(FSceneElementConditional::EmptyConditional);

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.StructItemSet.
			                                                 DatasmithSceneActorSet.
			                                                 Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.InnerStructItemSet.
			                                                 DatasmithSceneActorSet
			                                                 .Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.SoftDecorationItem.
			                                                 DatasmithSceneActorSet.
			                                                 Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
			                                                .Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.StructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
			                           );
			HideReplaceActorsSet.Append(FloorIter.Value->AllReference.SpaceItemSet.OtherItem);
		}
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.StructItemSet.
		                                            DatasmithSceneActorSet.
		                                            Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.InnerStructItemSet.
		                                            DatasmithSceneActorSet
		                                            .Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.SoftDecorationItem.
		                                            DatasmithSceneActorSet.
		                                            Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
		                                           .Array()
		                                      );

		HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.StructItemSet.OtherItem
		                           );
		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.InnerStructItemSet.OtherItem
		                           );
		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.SoftDecorationItem.OtherItem
		                           );
		HideReplaceActorsSet.Append(Iter.Value->AllReference.SpaceItemSet.OtherItem);
	}

	return false;
}

bool UGT_SwitchSceneElement_Space::ProcessTask_SwitchState()
{
	if (Super::ProcessTask_SwitchState())
	{
		return true;
	}

	FSceneElementConditional TempFilterTags;

	TempFilterTags.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_View);

	if (SceneElementPtr.IsValid())
	{
		SceneElementPtr->SwitchInteractionType(TempFilterTags);
	}

	return false;
}

void UGT_SwitchSceneElement_Device::Activate()
{
	Super::Activate();

	FilterTags.ConditionalSet = FGameplayTagContainer{Floor};
}

bool UGT_SwitchSceneElement_Device::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(Floor))
		{
			FloorIter.Value->SwitchInteractionType(FilterTags);

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

			const auto FloorIndex = FloorIter.Value->FloorIndex;

			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.InnerStructItemSet.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SoftDecorationItem.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SpaceItemSet.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );

			DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.StructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.SpaceItemSet.OtherItem
			                       );
		}
		else
		{
			FloorIter.Value->SwitchInteractionType(FSceneElementConditional::EmptyConditional);

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.StructItemSet.
			                                                 DatasmithSceneActorSet.
			                                                 Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.InnerStructItemSet.
			                                                 DatasmithSceneActorSet
			                                                 .Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.SoftDecorationItem.
			                                                 DatasmithSceneActorSet.
			                                                 Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
			                                                .Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.StructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
			                           );
			HideReplaceActorsSet.Append(FloorIter.Value->AllReference.SpaceItemSet.OtherItem);
		}
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.StructItemSet.
		                                            DatasmithSceneActorSet.
		                                            Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.InnerStructItemSet.
		                                            DatasmithSceneActorSet
		                                            .Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.SoftDecorationItem.
		                                            DatasmithSceneActorSet.
		                                            Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
		                                           .Array()
		                                      );

		HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.StructItemSet.OtherItem
		                           );
		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.InnerStructItemSet.OtherItem
		                           );
		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.SoftDecorationItem.OtherItem
		                           );
		HideReplaceActorsSet.Append(Iter.Value->AllReference.SpaceItemSet.OtherItem);
	}

	return false;
}

bool UGT_SwitchSceneElement_Device::ProcessTask_SwitchState()
{
	if (Super::ProcessTask_SwitchState())
	{
		return true;
	}

	FSceneElementConditional TempFilterTags;

	TempFilterTags.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_View);

	for (auto Iter : SceneElementSet)
	{
		Iter->SwitchInteractionType(TempFilterTags);
	}

	return false;
}

bool UGT_SwitchSceneElement_SpecialArea::ProcessTask_Display()
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorSet.Contains( FloorIter.Value->FloorTag))
		{
			FloorIter.Value->SwitchInteractionType(FilterTags);

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;

			const auto FloorIndex = FloorIter.Value->FloorIndex;

			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.InnerStructItemSet.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SoftDecorationItem.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SpaceItemSet.
			                                             DatasmithSceneActorSet.
			                                             Array()
			                                  );

			DataSmithSceneActorsSet.Append(TempDataSmithSceneActorsSet.Array());

			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.StructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
			                       );
			ReplaceActorsSet.Append(
			                        FloorIter.Value->AllReference.SpaceItemSet.OtherItem
			                       );
		}
		else
		{
			FloorIter.Value->SwitchInteractionType(FSceneElementConditional::EmptyConditional);

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.StructItemSet.
			                                                 DatasmithSceneActorSet.
			                                                 Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.InnerStructItemSet.
			                                                 DatasmithSceneActorSet
			                                                 .Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.SoftDecorationItem.
			                                                 DatasmithSceneActorSet.
			                                                 Array()
			                                      );
			TempHideDataSmithSceneActorsSet.Append(
			                                       FloorIter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
			                                                .Array()
			                                      );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.StructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem
			                           );
			HideReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.SoftDecorationItem.OtherItem
			                           );
			HideReplaceActorsSet.Append(FloorIter.Value->AllReference.SpaceItemSet.OtherItem);
		}
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->LandScapeHelper)
	{
		TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;

		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.StructItemSet.
		                                            DatasmithSceneActorSet.
		                                            Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.InnerStructItemSet.
		                                            DatasmithSceneActorSet
		                                            .Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.SoftDecorationItem.
		                                            DatasmithSceneActorSet.
		                                            Array()
		                                      );
		TempHideDataSmithSceneActorsSet.Append(
		                                       Iter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet
		                                           .Array()
		                                      );

		HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.StructItemSet.OtherItem
		                           );
		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.InnerStructItemSet.OtherItem
		                           );
		HideReplaceActorsSet.Append(
		                            Iter.Value->AllReference.SoftDecorationItem.OtherItem
		                           );
		HideReplaceActorsSet.Append(Iter.Value->AllReference.SpaceItemSet.OtherItem);
	}

	return false;
}

UGT_FloorSplit::UGT_FloorSplit(
	const FObjectInitializer& ObjectInitializer
	) :
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
	switch (Step)
	{
	case EStep::kSort:
		{
			if (ProcessTask_Sort())
			{
				return true;
			}
			else
			{
			}
			Step = EStep::kConfirmConditional;
			return true;
		}
	case EStep::kConfirmConditional:
		{
			if (ProcessTask_ConfirmConditional())
			{
				return true;
			}
			else
			{
			}

			UseScopeType = EUseScopeType::kNone;

			Step = EStep::kMove;
			return true;
		}
	case EStep::kMove:
		{
			if (ProcessTask_Move(DeltaTime))
			{
				return true;
			}
			else
			{
			}

			UseScopeType = EUseScopeType::kTime;

			Step = EStep::kDisplay;
			return true;
		}
	case EStep::kDisplay:
		{
			if (ProcessTask_Display())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kComplete;
			return true;
		}
	}

	return false;
}

bool UGT_FloorSplit::ProcessTask_Sort()
{
	// 要显示的DataSmith
	if (DataSmithSceneActorsSet.IsEmpty())
	{
		FGameplayTagContainer GameplayTagContainer;

		GameplayTagContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;
			TSet<TSoftObjectPtr<ASceneElementBase>> TempReplaceActorsSet;

			const auto FloorIndex = FloorIter.Value->FloorIndex;

			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.InnerStructItemSet.DatasmithSceneActorSet.
			                                             Array()
			                                  );

			DataSmithSceneActorsSet.Add(FloorIndex, TempDataSmithSceneActorsSet.Array());

			TempReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.StructItemSet.OtherItem.
			                                      Array()
			                           );
			TempReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem.
			                                      Array()
			                           );

			ReplaceActorsSet.Add(FloorIndex, TempReplaceActorsSet.Array());

			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempHideDataSmithSceneActorsSet;
			TSet<TSoftObjectPtr<ASceneElementBase>> TempHideReplaceActorsSet;

			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SoftDecorationItem.DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.SpaceItemSet.DatasmithSceneActorSet.Array()
			                                  );

			HideDataSmithSceneActorsSet.Append(TempHideDataSmithSceneActorsSet.Array());

			TempHideReplaceActorsSet.Append(FloorIter.Value->AllReference.SoftDecorationItem.OtherItem.Array());
			TempHideReplaceActorsSet.Append(FloorIter.Value->AllReference.SpaceItemSet.OtherItem.Array());

			HideReplaceActorsSet.Append(TempHideReplaceActorsSet.Array());
		}
	}

	return false;
}

bool UGT_FloorSplit::ProcessTask_ConfirmConditional()
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
			HideReplaceActorsSet[HideRePlaceActorsSetIndex]->GetAttachedActors(RelatedActors, true, true);
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
				Iter->GetAttachedActors(RelatedActors, true, true);
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

	return false;
}

bool UGT_FloorSplit::ProcessTask_Display()
{
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
	) :
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
	switch (Step)
	{
	case EStep::kSort:
		{
			if (ProcessTask_Sort())
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kMove;
			return true;
		}
	case EStep::kMove:
		{
			if (ProcessTask_Move(DeltaTime))
			{
				return true;
			}
			else
			{
			}

			Step = EStep::kComplete;
			return true;
		}
	}

	return false;
}

bool UGT_QuitFloorSplit::ProcessTask_Sort()
{
	// 要显示的DataSmith
	if (DataSmithSceneActorsSet.IsEmpty())
	{
		FGameplayTagContainer GameplayTagContainer;

		GameplayTagContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			TSet<TSoftObjectPtr<ADatasmithSceneActor>> TempDataSmithSceneActorsSet;
			TSet<TSoftObjectPtr<ASceneElementBase>> TempReplaceActorsSet;

			const auto FloorIndex = FloorIter.Value->FloorIndex;

			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.StructItemSet.DatasmithSceneActorSet.
			                                             Array()
			                                  );
			TempDataSmithSceneActorsSet.Append(
			                                   FloorIter.Value->AllReference.InnerStructItemSet.DatasmithSceneActorSet.
			                                             Array()
			                                  );

			DataSmithSceneActorsSet.Add(FloorIndex, TempDataSmithSceneActorsSet.Array());

			TempReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.StructItemSet.OtherItem.
			                                      Array()
			                           );
			TempReplaceActorsSet.Append(
			                            FloorIter.Value->AllReference.InnerStructItemSet.OtherItem.
			                                      Array()
			                           );

			ReplaceActorsSet.Add(FloorIndex, TempReplaceActorsSet.Array());
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
