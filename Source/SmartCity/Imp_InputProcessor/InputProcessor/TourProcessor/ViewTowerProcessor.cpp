#include "ViewTowerProcessor.h"

#include "Dynamic_WeatherBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GameOptions.h"
#include "GameplayTagsLibrary.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "TourPawn.h"
#include "TowerHelperBase.h"
#include "ViewerPawnBase.h"
#include "WeatherSystem.h"

TourProcessor::FViewTowerProcessor::FViewTowerProcessor(
	FOwnerPawnType* CharacterPtr
	) :
	  Super(CharacterPtr)
{
}

void TourProcessor::FViewTowerProcessor::EnterAction()
{
	FInputProcessor::EnterAction();

	// USceneInteractionWorldSystem::GetInstance()->SwitchDecoratorImp<FViewTower_Decorator>(
	// 	 USmartCitySuiteTags::Interaction_Area.GetTag(),
	// 	 USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag()
	// 	);

	AdjustCamera();

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
			UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateCloudCoverageMunualOverride(false);

			FDateTime Time(1, 1, 1, DecoratorSPtr->GetCurrentHour());
			UWeatherSystem::GetInstance()->AdjustTime(Time);
		}
	}

	SwitchShowCursor(true);

	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		auto GameOptionsPtr = UGameOptions::GetInstance();

		UpdateCameraArmLen(
		                   GameOptionsPtr->
		                   ViewTowerControlParam,
		                   0
		                  );

		UpdateCameraClampPitch(
		                       GameOptionsPtr->
		                       ViewTowerControlParam
		                      );
	}
}

bool TourProcessor::FViewTowerProcessor::InputKey(
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

bool TourProcessor::FViewTowerProcessor::InputAxis(
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
						                 ViewTowerControlParam.RotYawSpeed;
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
						                   ViewTowerControlParam.MoveSpeed;

						// OnwerActorPtr->AddMovementInput(
						//                                 Direction,
						//                                 Value
						//                                );

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
						                 ViewTowerControlParam.RotPitchSpeed;
						OnwerActorPtr->AddControllerPitchInput(-Rot);

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
						                   ViewTowerControlParam.MoveSpeed;

						// OnwerActorPtr->AddMovementInput(
						//                                 Direction,
						//                                 Value
						//                                );

						return true;
					}
				}
			}

			if (EventArgs.Key == GameOptionsPtr->MouseWheelAxis)
			{
				if (OnwerActorPtr->Controller != nullptr)
				{
					const auto Value = EventArgs.AmountDepressed * EventArgs.DeltaTime * GameOptionsPtr->
					                   ViewTowerControlParam.CameraSpringArmSpeed;

					const auto ClampValue = FMath::Clamp(
					                                     OnwerActorPtr->SpringArmComponent->TargetArmLength - Value,
					                                     GameOptionsPtr->
					                                     ViewTowerControlParam.MinCameraSpringArm,
					                                     GameOptionsPtr->
					                                     ViewTowerControlParam.MaxCameraSpringArm
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

void TourProcessor::FViewTowerProcessor::AdjustCameraSeat(
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
				                                                                          ATowerHelperBase::StaticClass(),
				                                                                          OutActors
				                                                                         );

			                                                                         for (auto ActorIter : OutActors)
			                                                                         {
				                                                                         auto TowerHelperPtr = Cast<
					                                                                         ATowerHelperBase>(
					                                                                          ActorIter
					                                                                         );
				                                                                         if (TowerHelperPtr)
				                                                                         {
					                                                                         if (TowerHelperPtr->
						                                                                         DefaultBuildingCameraSeat
						                                                                         .ToSoftObjectPath().
						                                                                         IsValid())
					                                                                         {
						                                                                         auto ViewerPawnPtr =
							                                                                         TowerHelperPtr->
							                                                                         DefaultBuildingCameraSeat
							                                                                         .LoadSynchronous();
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

void TourProcessor::FViewTowerProcessor::AdjustCamera()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformByPawnViewer>(
		 false,
		 [PCPtr](
		 UGT_CameraTransformByPawnViewer* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 TArray<AActor*> OutActors;
				 UGameplayStatics::GetAllActorsOfClass(
				                                       GTPtr,
				                                       ATowerHelperBase::StaticClass(),
				                                       OutActors
				                                      );

				 for (auto ActorIter : OutActors)
				 {
					 auto TowerHelperPtr = Cast<
						 ATowerHelperBase>(ActorIter);
					 if (TowerHelperPtr)
					 {
						 if (TowerHelperPtr->DefaultBuildingCameraSeat.ToSoftObjectPath().IsValid())
						 {
							 GTPtr->
								 ViewerPawnPtr
								 = TowerHelperPtr->DefaultBuildingCameraSeat.LoadSynchronous();
							 return;
						 }
					 }
				 }
			 }
		 }
		);
}
