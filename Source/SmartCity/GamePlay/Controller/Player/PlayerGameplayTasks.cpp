#include "PlayerGameplayTasks.h"

#include "DatasmithAssetUserData.h"
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

	FName Category = TEXT("Element*Category");
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

		auto PawnPtr = Cast<ATourPawn>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn());
		if (PawnPtr)
		{
			const auto Percent = CurrentTime / Duration;

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
	}
	else
	{
		EndTask();
	}
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

	for (; Index < ResultAry.Num();)
	{
		ON_SCOPE_EXIT
		{
			Index++;
		};

		auto Iter = ResultAry[Index];
		if (Iter->IsA(AStaticMeshActor::StaticClass()))
		{
		}
		else if (!Iter->GetComponents().IsEmpty())
		{
		}
		else
		{
			continue;
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
					if (ThirdIter.Key == FPrefix::Get().Datasmith_UniqueId)
					{
						SceneInteractionWorldSystemPtr->ItemRefMap.Add(FGuid(ThirdIter.Key.ToString()), Iter);
					}
				}
				break;
			}
		}

		PRINTINVOKEWITHSTR(FString::Printf(TEXT("%d %d"), Index, ResultAry.Num()));
		
		return;
	}

	EndTask();
}

void UGT_InitializeSceneActors::OnDestroy(
	bool bInOwnerFinished
	)
{
	Super::OnDestroy(bInOwnerFinished);
}

UGT_SceneObjSwitch::UGT_SceneObjSwitch(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
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
	
	for (; Index < ResultAry.Num();)
	{
		ON_SCOPE_EXIT
		{
			Index++;
		};

		Check(ResultAry[Index]);
		
		PRINTINVOKEWITHSTR(FString::Printf(TEXT("%d %d"), Index, ResultAry.Num()));

		CurrentTickProcessNum++;
		if (CurrentTickProcessNum < PerTickProcessNum)
		{
			
		}
		else
		{
			return;
		}
	}

	EndTask();
}

void UGT_SceneObjSwitch::OnDestroy(
	bool bInOwnerFinished
	)
{
	OnEnd.Broadcast(true, Result);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UGT_SceneObjSwitch::Check(AActor* Actor)
{
	if (SceneInteractionWorldSystemPtr->SceneActorsRefMap.Contains(Actor))
	{
	}
	else
	{
		PRINTINVOKEWITHSTR(FString(TEXT("")));
		return;
	}

	if (Actor->IsA(ALight::StaticClass()))
	{
		PRINTINVOKEWITHSTR(FString(TEXT("")));
		Actor->SetActorHiddenInGame(true);
		return;
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
			return;
		}
	}

	PRINTINVOKEWITHSTR(FString(TEXT("")));
	Actor->SetActorHiddenInGame(false);

	Result.Add(Actor);
}
