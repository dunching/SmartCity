#include "ViewBuildingProcessor.h"

#include "Dynamic_WeatherBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GameOptions.h"
#include "GameplayTagsLibrary.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewerPawn.h"
#include "TourPawn.h"
#include "WeatherSystem.h"

TourProcessor::FViewBuildingProcessor::FViewBuildingProcessor(
	FOwnerPawnType* CharacterPtr
	) :
	  Super(CharacterPtr)
{
}

void TourProcessor::FViewBuildingProcessor::EnterAction()
{
	FInputProcessor::EnterAction();

	USceneInteractionWorldSystem::GetInstance()->SwitchDecoratorImp<FExternalWall_Decorator>(
		 USmartCitySuiteTags::Interaction_Area.GetTag(),
		 USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag(),
		 USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag()
		);

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ReplyCameraTransform>(
	false,
		 [](
		 UGT_ReplyCameraTransform* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->SeatTag = USmartCitySuiteTags::Seat_Default;
			 }
		 }
		);

	{
		// 确认当前的模式
		auto DecoratorSPtr =
			DynamicCastSharedPtr<FInteraction_Decorator>(
														 USceneInteractionWorldSystem::GetInstance()->
														 GetDecorator(
																	  USmartCitySuiteTags::Interaction_Interaction
																	 )
														);
		if (DecoratorSPtr)
		{
			UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(DecoratorSPtr->GetCurrentWeather());

			FDateTime Time(1, 1, 1, DecoratorSPtr->GetCurrentHour());
			UWeatherSystem::GetInstance()->AdjustTime(Time);
		}
	}
	
	SwitchShowCursor(true);

	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		OnwerActorPtr->UpdateControlParam(UGameOptions::GetInstance()->ViewBuildingControlParam);
	}
}

bool TourProcessor::FViewBuildingProcessor::InputKey(
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

bool TourProcessor::FViewBuildingProcessor::InputAxis(
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
						                 ViewBuildingControlParam.RotYawSpeed;
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
						                   ViewBuildingControlParam.MoveSpeed;

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
						                 ViewBuildingControlParam.RotPitchSpeed;
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
						                   ViewBuildingControlParam.MoveSpeed;

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
					                   ViewBuildingControlParam.CameraSpringArmSpeed;

					const auto ClampValue = FMath::Clamp(
					                                     OnwerActorPtr->SpringArmComponent->TargetArmLength - Value,
					                                     GameOptionsPtr->
					                                     ViewBuildingControlParam.MinCameraSpringArm,
					                                     GameOptionsPtr->
					                                     ViewBuildingControlParam.MaxCameraSpringArm
					                                    );

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

void TourProcessor::FViewBuildingProcessor::AdjustCameraSeat(
	const FRotator& CameraSeat
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransform>(
	false,
	                                                                         [PCPtr, &CameraSeat](
	                                                                         UGT_CameraTransform* GTPtr
	                                                                         )
	                                                                         {
		                                                                         if (GTPtr)
		                                                                         {
			                                                                         TArray<AActor*> OutActors;
			                                                                         UGameplayStatics::GetAllActorsOfClass(
				                                                                          GTPtr,
				                                                                          AViewerPawn::StaticClass(),
				                                                                          OutActors
				                                                                         );

			                                                                         for (auto ActorIter : OutActors)
			                                                                         {
				                                                                         auto ViewerPawnPtr = Cast<
					                                                                         AViewerPawn>(ActorIter);
				                                                                         if (ViewerPawnPtr)
				                                                                         {
					                                                                         if (ViewerPawnPtr->SeatTag
						                                                                         ==
						                                                                         USmartCitySuiteTags::Seat_Default)
					                                                                         {
						                                                                         GTPtr->TargetLocation =
							                                                                         ViewerPawnPtr->
							                                                                         GetActorLocation();

						                                                                         GTPtr->TargetRotation =
							                                                                         FRotator(
								                                                                          CameraSeat.
								                                                                          Pitch,
								                                                                          ViewerPawnPtr
								                                                                          ->
								                                                                          GetActorRotation()
								                                                                          .Yaw,
								                                                                          0
								                                                                         );

						                                                                         GTPtr->
							                                                                         TargetTargetArmLength
							                                                                         = ViewerPawnPtr->
							                                                                         SpringArmComponent
							                                                                         ->TargetArmLength;

						                                                                         return;
					                                                                         }
				                                                                         }
			                                                                         }
		                                                                         }
	                                                                         }
	                                                                        );
}
