#include "PlayerGameplayTasks.h"

#include "Algorithm.h"
#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
#include "GameOptions.h"
#include "SceneInteractionWorldSystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Tasks/AITask.h"

#include "TourPawn.h"
#include "ViewerPawn.h"
#include "Tools.h"
#include "GameplayTagsLibrary.h"
#include "LogWriter.h"
#include "TemplateHelper.h"
#include "Engine/Light.h"

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
		                                              UGameOptions::GetInstance()->ViewDeviceFOV
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
}

void UGT_InitializeSceneActors::Activate()
{
	Super::Activate();

	UGameplayStatics::GetAllActorsOfClass(
	                                      GetWorldImp(),
	                                      AActor::StaticClass(),
	                                      ResultAry
	                                     );
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
	ON_SCOPE_EXIT
	{
		Index++;
	};

	if (Index >= ResultAry.Num())
	{
		return false;
	}

	auto Iter = ResultAry[Index];
	if (Iter->IsA(AStaticMeshActor::StaticClass()))
	{
	}
	else if (!Iter->GetComponents().IsEmpty())
	{
	}
	else
	{
		return true;
	}

	if (Iter->ActorHasTag(TEXT("All")))
	{
		SceneInteractionWorldSystemPtr->SceneActorsRefMap.Add(
		                                                      Iter,
		                                                      {UGameplayTagsLibrary::Interaction_Area_ExternalWall}
		                                                     );
	}
	else if (Iter->ActorHasTag(TEXT("F1")))
	{
		SceneInteractionWorldSystemPtr->SceneActorsRefMap.Add(
		                                                      Iter,
		                                                      {UGameplayTagsLibrary::Interaction_Area_Floor_F1}
		                                                     );
	}

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
				PRINTINVOKEWITHSTR(FString::Printf(TEXT("%s %s"), *ThirdIter.Key.ToString(), *ThirdIter.Value));
				if (ThirdIter.Key == UAssetRefMap::GetInstance()->Datasmith_UniqueId)
				{
					SceneInteractionWorldSystemPtr->ItemRefMap.Add(FGuid(ThirdIter.Value), Iter);
					continue;
				}

				auto CatogoryPrefixIter = UAssetRefMap::GetInstance()->CatogoryPrifix.Find(ThirdIter.Key.ToString());
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

	PRINTINVOKEWITHSTR(FString::Printf(TEXT("%d %d"), Index, ResultAry.Num()));

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

	// 更新过滤条件
	Filters.Add(
	            DecoratorType,
	            FilterTags
	           );

	UGameplayStatics::GetAllActorsOfClass(
	                                      this,
	                                      AActor::StaticClass(),
	                                      ResultAry
	                                     );
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
	ON_SCOPE_EXIT
	{
		Index++;
	};

	if (Index >= ResultAry.Num())
	{
		return false;
	}

	auto Actor = ResultAry[Index];
	if (SceneInteractionWorldSystemPtr->SceneActorsRefMap.Contains(Actor))
	{
	}
	else
	{
		PRINTINVOKEWITHSTR(FString(TEXT("")));
		return true;
	}

	if (Actor->IsA(ALight::StaticClass()))
	{
		PRINTINVOKEWITHSTR(FString(TEXT("")));
		Actor->SetActorHiddenInGame(true);
		return true;
	}

	auto Filter = SceneInteractionWorldSystemPtr->SceneActorsRefMap[Actor];

	TSet<FGameplayTag> FilterSet;
	for (const auto& Iter : Filters)
	{
		FilterSet.Append(Iter.Value);
	}

	for (const auto& Iter : FilterSet)
	{
		if (Filter.Contains(Iter))
		{
		}
		else
		{
			PRINTINVOKEWITHSTR(FString(TEXT("")));
			Actor->SetActorHiddenInGame(true);
			return true;
		}
	}

	PRINTINVOKEWITHSTR(FString(TEXT("")));
	Actor->SetActorHiddenInGame(false);

	Result.Add(Actor);

	PRINTINVOKEWITHSTR(FString::Printf(TEXT("%d %d"), Index, ResultAry.Num()));

	return true;
}
