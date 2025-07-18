#include "TourProcessor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameOptions.h"
#include "ViewerPawn.h"
#include "TourPawn.h"

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

			if (EventArgs.Key == GameOptionsPtr->MoveBtn)
			{
				bStartMove = true;
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

			if (EventArgs.Key == GameOptionsPtr->MoveBtn)
			{
				bStartMove = false;
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
						const auto Rot = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->RotYawSpeed;
						OnwerActorPtr->AddControllerYawInput(Rot);

						return true;
					}
					else if (bStartMove)
					{
						const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

						const FVector Direction = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector,
							Rotation.Quaternion().GetRightVector()).Vector();

						const auto Value = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
							MoveSpeed;

						OnwerActorPtr->AddMovementInput(Direction,
						                                Value);

						return true;
					}
				}
			}

			if (EventArgs.Key == GameOptionsPtr->MouseY)
			{
				if (OnwerActorPtr->Controller != nullptr)
				{
					if (bStartRot)
					{
						const auto Rot = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
							RotPitchSpeed;
						OnwerActorPtr->AddControllerPitchInput(Rot);

						return true;
					}
					else if (bStartMove)
					{
						const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

						const FVector Direction = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector,
							Rotation.Quaternion().GetForwardVector()).Vector();

						const auto Value = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
							MoveSpeed;

						OnwerActorPtr->AddMovementInput(Direction,
						                                Value);

						return true;
					}
				}
			}

			if (EventArgs.Key == GameOptionsPtr->MouseWheelAxis)
			{
				if (OnwerActorPtr->Controller != nullptr)
				{
					const auto Value = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
						CameraSpringArmSpeed;

					const auto ClampValue = FMath::Clamp(OnwerActorPtr->SpringArmComponent->TargetArmLength - Value,
					                                     GameOptionsPtr->
					                                     MinCameraSpringArm,
					                                     GameOptionsPtr->
					                                     MaxCameraSpringArm);

					OnwerActorPtr->SpringArmComponent->TargetArmLength = ClampValue;

					return true;
				}
			}
		}
		break;
	default: ;
	}

	return Super::InputKey(EventArgs);
}
