#include "TourProcessor.h"

#include "GameOptions.h"
#include "ViewerPawn.h"

inline TourProcessor::FTourProcessor::FTourProcessor(FOwnerPawnType* CharacterPtr):
	Super(CharacterPtr)
{
}

void TourProcessor::FTourProcessor::EnterAction()
{
	FInputProcessor::EnterAction();

	SwitchShowCursor(true);
}

bool TourProcessor::FTourProcessor::InputKey(const FInputKeyEventArgs& EventArgs)
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
				bStartRot = true;
				return true;
			}

			if (EventArgs.Key == GameOptionsPtr->ClickItem)
			{
				return true;
			}
		}
		break;
	case IE_Released:
		{
			auto GameOptionsPtr = UGameOptions::GetInstance();

			if (EventArgs.Key == GameOptionsPtr->RotBtn)
			{
				bStartRot = false;
				return true;
			}
		}
		break;
	default: ;
	}

	return Super::InputKey(EventArgs);
}

bool TourProcessor::FTourProcessor::InputAxis(const FInputKeyEventArgs& EventArgs)
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
				if (OnwerActorPtr->Controller != nullptr)
				{
					if (bStartRot)
					{
						OnwerActorPtr->AddControllerYawInput(EventArgs.DeltaTime);
					}
					else if (bStartMove)
					{
						const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

						const FVector RightDirection = Rotation.Quaternion().GetRightVector();

						OnwerActorPtr->AddMovementInput(RightDirection, EventArgs.DeltaTime);
					}
				}

				return true;
			}

			if (EventArgs.Key == GameOptionsPtr->MouseX)
			{
				return true;
			}
		}
		break;
	default: ;
	}

	return Super::InputKey(EventArgs);
}
