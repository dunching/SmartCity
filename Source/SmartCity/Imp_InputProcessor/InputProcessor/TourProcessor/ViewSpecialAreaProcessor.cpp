#include "ViewSpecialAreaProcessor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameOptions.h"
#include "GameplayTagsLibrary.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TourPawn.h"

TourProcessor::FViewSpecialAreaProcessor::FViewSpecialAreaProcessor(
	FOwnerPawnType* CharacterPtr
	) :
	  Super(CharacterPtr)
{
}

void TourProcessor::FViewSpecialAreaProcessor::EnterAction()
{
	FInputProcessor::EnterAction();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ReplyCameraTransform>(
		 false,
		 [](
		 UGT_ReplyCameraTransform* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->SeatTag = USmartCitySuiteTags::Seat_ViewSplit;
			 }
		 }
		);

	SwitchShowCursor(true);

	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		auto GameOptionsPtr = UGameOptions::GetInstance();

		OnwerActorPtr->UpdateControlParam(GameOptionsPtr->ViewSpaceControlParam);
		
		UpdateCameraArmLen(
						   GameOptionsPtr->
						   ViewSpaceControlParam,
						   0
						  );

		UpdateCameraClampPitch(
							   GameOptionsPtr->
							   ViewSpaceControlParam
							  );
	}
}

bool TourProcessor::FViewSpecialAreaProcessor::InputKey(
	const FInputKeyEventArgs& EventArgs
	)
{
	switch (EventArgs.Event)
	{
	case IE_Pressed:
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (!OnwerActorPtr)
			{
				return false;
			}

			auto GameOptionsPtr = UGameOptions::GetInstance();

			if (EventArgs.Key == GameOptionsPtr->RotBtn)
			{
				bHasRoted = false;

				bStartRot = true;
				return true;
			}

			if (EventArgs.Key == GameOptionsPtr->MoveBtn)
			{
				bHasMoved = false;

				bStartMove = true;
				return true;
			}
		}
		break;
	case IE_Released:
		{
			auto GameOptionsPtr = UGameOptions::GetInstance();

			if (EventArgs.Key == GameOptionsPtr->ClickItem)
			{
				if (bHasRoted || bHasMoved)
				{
				}
				else
				{
					// 如果未旋转或移动、则继续往下
					USceneInteractionWorldSystem::GetInstance()->Operation(EOperatorType::kLeftMouseButton);
				}
			}

			if (EventArgs.Key == GameOptionsPtr->RotBtn)
			{
				bHasRoted = false;

				bStartRot = false;
				return true;
			}

			if (EventArgs.Key == GameOptionsPtr->MoveBtn)
			{
				bHasMoved = false;

				bStartMove = false;
				return true;
			}
		}
		break;
	default: ;
	}

	return Super::InputKey(EventArgs);
}

bool TourProcessor::FViewSpecialAreaProcessor::InputAxis(
	const FInputKeyEventArgs& EventArgs
	)
{
	switch (EventArgs.Event)
	{
	case IE_Axis:
		{
			auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
			if (!OnwerActorPtr)
			{
				return false;
			}

			auto GameOptionsPtr = UGameOptions::GetInstance();

			if (EventArgs.Key == GameOptionsPtr->MouseX)
			{
				// 这个状态下不进行此操作
			}

			if (EventArgs.Key == GameOptionsPtr->MouseY)
			{
				// 这个状态下不进行此操作
			}

			if (EventArgs.Key == GameOptionsPtr->MouseWheelAxis)
			{
				// 这个状态下不进行此操作
			}
		}
		break;
	default: ;
	}

	return Super::InputKey(EventArgs);
}
