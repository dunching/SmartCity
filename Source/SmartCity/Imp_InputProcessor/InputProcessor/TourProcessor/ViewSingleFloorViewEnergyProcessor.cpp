#include "ViewSingleFloorViewEnergyProcessor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameOptions.h"
#include "GameplayTagsLibrary.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "ViewerPawn.h"
#include "TourPawn.h"

TourProcessor::FViewSingleFloorViewEnergyProcessor::FViewSingleFloorViewEnergyProcessor(
	FOwnerPawnType* CharacterPtr
	):
	 Super(CharacterPtr)
{
}

void TourProcessor::FViewSingleFloorViewEnergyProcessor::EnterAction()
{
	FInputProcessor::EnterAction();

	SwitchShowCursor(true);
	
	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		OnwerActorPtr->UpdateControlParam(UGameOptions::GetInstance()->ViewFloorEnergyControlParam);
		
		auto GameOptionsPtr = UGameOptions::GetInstance();

		UpdateCameraArmLen(GameOptionsPtr->
						   ViewFloorEnergyControlParam, 0);

		UpdateCamera(GameOptionsPtr->
						   ViewFloorEnergyControlParam);
					
	}
}

bool TourProcessor::FViewSingleFloorViewEnergyProcessor::InputKey(
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

bool TourProcessor::FViewSingleFloorViewEnergyProcessor::InputAxis(
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
				if (OnwerActorPtr->Controller != nullptr)
				{
					if (bStartRot)
					{
						bHasRoted = true;

						const auto Rot = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
						                 ViewFloorControlParam.RotYawSpeed;
						OnwerActorPtr->AddControllerYawInput(Rot);

						return true;
					}
					else if (bStartMove)
					{
						bHasMoved = true;

						const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

						const FVector Direction = UKismetMathLibrary::MakeRotFromZX(
							 FVector::UpVector,
							 Rotation.Quaternion().GetRightVector()
							).Vector();

						const auto Value = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
						                   ViewFloorControlParam.MoveSpeed;

						OnwerActorPtr->AddMovementInput(
						                                Direction,
						                                Value
						                               );

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
						bHasRoted = true;

						const auto Rot = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
						                 ViewFloorControlParam.RotPitchSpeed;
						OnwerActorPtr->AddControllerPitchInput(Rot);

						return true;
					}
					else if (bStartMove)
					{
						bHasMoved = true;

						const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

						const FVector Direction = UKismetMathLibrary::MakeRotFromZX(
							 FVector::UpVector,
							 Rotation.Quaternion().GetForwardVector()
							).Vector();

						const auto Value = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
						                   ViewFloorControlParam.MoveSpeed;

						OnwerActorPtr->AddMovementInput(
						                                Direction,
						                                Value
						                               );

						return true;
					}
				}
			}

			if (EventArgs.Key == GameOptionsPtr->MouseWheelAxis)
			{
				if (OnwerActorPtr->Controller != nullptr)
				{
					const auto Value = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
					                   ViewFloorEnergyControlParam.CameraSpringArmSpeed;

					UpdateCameraArmLen(GameOptionsPtr->
									   ViewFloorEnergyControlParam, Value);
					
					return true;
				}
			}
		}
		break;
	default: ;
	}

	return Super::InputKey(EventArgs);
}
