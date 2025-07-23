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
#include "TemplateHelper.h"

struct FPrefix : public TStructVariable<FPrefix>
{
	FName Datasmith_UniqueId = TEXT("Datasmith_UniqueId");
};

FName UPlayerControllerGameplayTasksComponent::ComponentName = TEXT("PlayerControllerGameplayTasksComponent");

inline UGT_ReplyCameraTransform::UGT_ReplyCameraTransform(
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

	if (SceneActorMap.IsValidIndex(0))
	{
		for (const auto& Iter : SceneActorMap[0].DataSmithSceneActorsSet)
		{
			DataSmithSceneActorsSet.Add(Iter);
		}
	}

	if (DataSmithSceneActorsSet.IsValidIndex(0))
	{
		for (const auto& Iter : DataSmithSceneActorsSet[0]->RelatedActors)
		{
			RelatedActors.Add({Iter.Key, Iter.Value});
		}
	}
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

	if (DataSmithSceneActorsSetIndex < DataSmithSceneActorsSet.Num())
	{
	}
	else
	{
		DataSmithSceneActorsSetIndex = 0;
		SceneActorMapIndex++;

		if (SceneActorMapIndex < SceneActorMap.Num())
		{
			DataSmithSceneActorsSet.Empty();
			for (const auto& Iter : SceneActorMap[SceneActorMapIndex].DataSmithSceneActorsSet)
			{
				DataSmithSceneActorsSet.Add(Iter);
			}

			if (DataSmithSceneActorsSet.IsValidIndex(0))
			{
				for (const auto& Iter : DataSmithSceneActorsSet[0]->RelatedActors)
				{
					RelatedActors.Add({Iter.Key, Iter.Value});
				}
			}
		}

		return true;
	}

	if (RelatedActorsIndex < RelatedActors.Num())
	{
	}
	else
	{
		RelatedActorsIndex = 0;
		DataSmithSceneActorsSetIndex++;

		if (DataSmithSceneActorsSetIndex < DataSmithSceneActorsSet.Num())
		{
			RelatedActors.Empty();
			for (const auto& Iter : DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex]->RelatedActors)
			{
				RelatedActors.Add({Iter.Key, Iter.Value});
			}
		}

		return true;
	}

	ON_SCOPE_EXIT
	{
		RelatedActorsIndex++;
	};

	auto Iter = RelatedActors[RelatedActorsIndex].Value;
	auto Components = Iter->GetComponents();
	for (auto SecondIterr : Components)
	{
		auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIterr);
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
					SceneInteractionWorldSystemPtr->ItemRefMap.Add(FGuid(ThirdIter.Value), Iter.LoadSynchronous());
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
						auto STCPTr = Iter->FindComponentByClass<UStaticMeshComponent>();
						if (STCPTr)
						{
							STCPTr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
							STCPTr->SetCollisionObjectType(Device_Object);
							STCPTr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
						}
						continue;
					}
					else if (ThirdIter.Value == UAssetRefMap::GetInstance()->XFJZ)
					{
						auto STCPTr = Iter->FindComponentByClass<UStaticMeshComponent>();
						if (STCPTr)
						{
							STCPTr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
							STCPTr->SetCollisionObjectType(Device_Object);
							STCPTr->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
						}
						continue;
					}
					else
					{
						auto STCPTr = Iter->FindComponentByClass<UStaticMeshComponent>();
						if (STCPTr)
						{
							STCPTr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						}
						continue;
					}
				}
			}
			break;
		}
	}

	return true;
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
				DataSmithSceneActorsSet.Append(SecondIter.Value.DataSmithSceneActorsSet.Array());
			}
			else
			{
				HideDataSmithSceneActorsSet.Append(SecondIter.Value.DataSmithSceneActorsSet.Array());
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
			for (const auto& Iter : HideDataSmithSceneActorsSet[HideDataSmithSceneActorsSetIndex]->RelatedActors)
			{
				FilterCount.emplace(Iter.Value.LoadSynchronous(), 0);
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
			for (const auto& Iter : DataSmithSceneActorsSet[DataSmithSceneActorsSetIndex]->RelatedActors)
			{
				FilterCount[Iter.Value.LoadSynchronous()] = 1;
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
