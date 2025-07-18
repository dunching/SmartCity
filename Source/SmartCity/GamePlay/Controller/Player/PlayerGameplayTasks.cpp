#include "PlayerGameplayTasks.h"

#include "TourPawn.h"
#include "ViewerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

FName UPlayerControllerGameplayTasksComponent::ComponentName = TEXT("PlayerControllerGameplayTasksComponent");

inline UGT_ReplyCameraTransform::UGT_ReplyCameraTransform(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGT_ReplyCameraTransform::Activate()
{
	Super::Activate();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this,
	                                      AViewerPawn::StaticClass(),
	                                      OutActors);
	for (auto ActorIter : OutActors)
	{
		auto ViewerPawnPtr = Cast<AViewerPawn>(ActorIter);
		if (ViewerPawnPtr)
		{
			if (ViewerPawnPtr->SeatTag == SeatTag)
			{
				auto PCPtr = GEngine->GetFirstLocalPlayerController(GetWorld());
				auto PawnPtr = Cast<ATourPawn>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn());
				if (PawnPtr)
				{
					OriginalLocation = ViewerPawnPtr->GetActorLocation();
					OriginalRotation = PCPtr->GetControlRotation();
					OriginalSpringArmLen = PawnPtr->SpringArmComponent->TargetArmLength;

					TargetLocation = ViewerPawnPtr->GetActorLocation();
					TargetRotation = ViewerPawnPtr->GetActorRotation();
					TargetTargetArmLength = ViewerPawnPtr->SpringArmComponent->TargetArmLength;

					return;
				}
			}
		}
	}

	EndTask();
}

void UGT_ReplyCameraTransform::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (CurrentTime < Duration)
	{
		CurrentTime += DeltaTime;

		auto PawnPtr = Cast<ATourPawn>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn());
		if (PawnPtr)
		{
			const auto Percent = CurrentTime / Duration;

			const auto CurrentLocation = UKismetMathLibrary::VLerp(OriginalLocation,
			                                                       TargetLocation,
			                                                       Percent);

			const auto CurrentRotation = UKismetMathLibrary::RLerp(OriginalRotation,
			                                                       TargetRotation,
			                                                       Percent,
			                                                       true);

			const auto CurrentTargetArmLength = FMath::Lerp(
				OriginalSpringArmLen,
				TargetTargetArmLength,
				Percent);

			PawnPtr->LerpToSeat(FTransform(CurrentRotation,
			                               CurrentLocation),
			                    CurrentTargetArmLength);
		}
	}
	else
	{
		EndTask();
	}
}

void UGT_ReplyCameraTransform::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
