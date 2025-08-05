#include "SceneInteractionDecorator.h"

#include "SceneElement_AccessControl.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/WebChannelWorldSystem.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameOptions.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "SceneInteractionWorldSystem.h"
#include "Algorithm.h"
#include "Elevator.h"
#include "GameplayCommand.h"
#include "GameplayTagsLibrary.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"

FDecoratorBase::FDecoratorBase(
	FGameplayTag InMainDecoratorType,
	FGameplayTag InBranchDecoratorType
	):
	 MainDecoratorType(InMainDecoratorType)
	 , BranchDecoratorType(InBranchDecoratorType)
{
}

FDecoratorBase::~FDecoratorBase()
{
}

void FDecoratorBase::Entry()
{
}

void FDecoratorBase::Quit()
{
}

bool FDecoratorBase::NeedAsync() const
{
	return false;
}

void FDecoratorBase::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

void FDecoratorBase::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

bool FDecoratorBase::Operation(
	EOperatorType OperatorType
	)
{
	return false;
}

FGameplayTag FDecoratorBase::GetMainDecoratorType() const
{
	return MainDecoratorType;
}

FGameplayTag FDecoratorBase::GetBranchDecoratorType() const
{
	return BranchDecoratorType;
}

void FDecoratorBase::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
}

FEmpty_Decorator::FEmpty_Decorator():
                                    Super(
                                          UGameplayTagsLibrary::Interaction_Mode,
                                          UGameplayTagsLibrary::Interaction_Mode_Empty
                                         )
{
}

void FTour_Decorator::Entry()
{
	Super::Entry();
}

FTour_Decorator::FTour_Decorator():
                                  Super(
                                        UGameplayTagsLibrary::Interaction_Mode,
                                        UGameplayTagsLibrary::Interaction_Mode_Tour
                                       )
{
}

bool FTour_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	PRINTFUNCSTR();

	return Super::Operation(OperatorType);
}

FSceneMode_Decorator::FSceneMode_Decorator():
                                            Super(
                                                  UGameplayTagsLibrary::Interaction_Mode,
                                                  UGameplayTagsLibrary::Interaction_Mode_Scene
                                                 )
{
}

void FSceneMode_Decorator::Entry()
{
	Super::Entry();
}

bool FSceneMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	PRINTFUNCSTR();

	return Super::Operation(OperatorType);
}

FRDRadarMode_Decorator::FRDRadarMode_Decorator():
                                                Super(
                                                      UGameplayTagsLibrary::Interaction_Mode,
                                                      UGameplayTagsLibrary::Interaction_Mode_ELV_Radar
                                                     )
{
}

FRDRadarMode_Decorator::~FRDRadarMode_Decorator()
{
}

void FRDRadarMode_Decorator::Entry()
{
	Super::Entry();

	GetWorldImp()->GetTimerManager().SetTimer(
	                                          QueryTimerHadnle,
	                                          std::bind(&ThisClass::RadarQuery, this),
	                                          UGameOptions::GetInstance()->RadarQueryFrequency,
	                                          true
	                                         );
}

void FRDRadarMode_Decorator::Quit()
{
	GetWorldImp()->GetTimerManager().ClearTimer(
	                                            QueryTimerHadnle
	                                           );

	Super::Quit();
}

bool FRDRadarMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

void FRDRadarMode_Decorator::RadarQuery()
{
}

FQDMode_Decorator::FQDMode_Decorator():
                                      Super(
                                            UGameplayTagsLibrary::Interaction_Mode,
                                            UGameplayTagsLibrary::Interaction_Mode_PWR
                                           )
{
}

FAccessControlMode_Decorator::FAccessControlMode_Decorator():
                                                            Super(
                                                                  UGameplayTagsLibrary::Interaction_Mode,
                                                                  UGameplayTagsLibrary::Interaction_Mode_ELV_AccessControl
                                                                 )
{
}

void FAccessControlMode_Decorator::Entry()
{
	Super::Entry();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ASceneElement_AccessControl::StaticClass(), OutActors);

	USceneInteractionWorldSystem::GetInstance()->ClearFocus();
	USceneInteractionWorldSystem::GetInstance()->ClearRouteMarker();

	for (auto Iter : OutActors)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(Iter, EInteractionType::kFocus);
	}
}

FElevatorMode_Decorator::FElevatorMode_Decorator():
                                                  Super(
                                                        UGameplayTagsLibrary::Interaction_Mode,
                                                        UGameplayTagsLibrary::Interaction_Mode_Elevator
                                                       )
{
}

void FElevatorMode_Decorator::Entry()
{
	Super::Entry();

	for (auto Iter : UAssetRefMap::GetInstance()->ElevatorMap)
	{
		if (Iter.Value.ToSoftObjectPath().IsValid())
		{
			Iter.Value->SwitchState(true);
		}
	}
}

void FElevatorMode_Decorator::Quit()
{
	for (auto Iter : UAssetRefMap::GetInstance()->ElevatorMap)
	{
		if (Iter.Value.ToSoftObjectPath().IsValid())
		{
			Iter.Value->SwitchState(false);
		}
	}

	Super::Quit();
}

FArea_Decorator::FArea_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       UGameplayTagsLibrary::Interaction_Area,
	       Interaction_Area
	      )
	 , CurrentInteraction_Area(Interaction_Area)
{
}

void FArea_Decorator::Entry()
{
	Super::Entry();

	auto DecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetDecorator(
		 UGameplayTagsLibrary::Interaction_Mode
		);
	if (
		DecoratorSPtr &&
		!DecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Empty)
		)
	{
		TSet<FSceneElementConditional, TSceneElementConditionalKeyFuncs> FilterTags;

		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

			FilterTags.Add(SceneActorConditional);
		}

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
		                                                          FilterTags,
		                                                          std::bind(
		                                                                    &ThisClass::OnUpdateFilterComplete,
		                                                                    this,
		                                                                    std::placeholders::_1,
		                                                                    std::placeholders::_2
		                                                                   )
		                                                         );
	}
	else
	{
		TSet<FSceneElementConditional, TSceneElementConditionalKeyFuncs> FilterTags;

		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			FilterTags.Add(SceneActorConditional);
		}

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
		                                                          FilterTags,
		                                                          std::bind(
		                                                                    &ThisClass::OnUpdateFilterComplete,
		                                                                    this,
		                                                                    std::placeholders::_1,
		                                                                    std::placeholders::_2
		                                                                   )
		                                                         );
	}
}

void FArea_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode) &&
		!NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Empty)
		)
	{
		TSet<FSceneElementConditional, TSceneElementConditionalKeyFuncs> FilterTags;

		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			FilterTags.Add(SceneActorConditional);
		}

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
		                                                          FilterTags
		                                                         );
	}
	else
	{
		TSet<FSceneElementConditional, TSceneElementConditionalKeyFuncs> FilterTags;

		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			FilterTags.Add(SceneActorConditional);
		}

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
		                                                          FilterTags,
		                                                          std::bind(
		                                                                    &ThisClass::OnUpdateFilterComplete,
		                                                                    this,
		                                                                    std::placeholders::_1,
		                                                                    std::placeholders::_2
		                                                                   )
		                                                         );
	}
}

void FArea_Decorator::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorQuit(NewDecoratorSPtr);
}

void FArea_Decorator::UpdateDisplay()
{
}

void FArea_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
}

FExternalWall_Decorator::FExternalWall_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       Interaction_Area
	      )
{
}

void FExternalWall_Decorator::Entry()
{
	Super::Entry();
}

bool FExternalWall_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	Super::Operation(OperatorType);

	PRINTFUNCSTR();

	switch (OperatorType)
	{
	case EOperatorType::kLeftMouseButton:
	case EOperatorType::kRightMouseButton:
		{
		}
		break;
	case EOperatorType::kNone:
		break;
	default: ;
	}

	return false;
}

FSplitFloor_Decorator::FSplitFloor_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       UGameplayTagsLibrary::Interaction_Area,
	       Interaction_Area
	      )
{
}

void FSplitFloor_Decorator::Entry()
{
	Super::Entry();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_FloorSplit>(
	                                                                    [this](
	                                                                    UGT_FloorSplit* GTPtr
	                                                                    )
	                                                                    {
	                                                                    }
	                                                                   );
}

void FSplitFloor_Decorator::Quit()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_QuitFloorSplit>(
	                                                                        [this](
	                                                                        auto GTPtr
	                                                                        )
	                                                                        {
		                                                                        GTPtr->OnEnd.AddLambda(
			                                                                         [this](
			                                                                         auto
			                                                                         )
			                                                                         {
				                                                                         OnAsyncQuitComplete.
					                                                                         ExecuteIfBound();
			                                                                         }
			                                                                        );
	                                                                        }
	                                                                       );

	Super::Quit();
}

bool FSplitFloor_Decorator::NeedAsync() const
{
	return true;
}

bool FSplitFloor_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

FFloor_Decorator::FFloor_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       Interaction_Area
	      )
{
}

FFloor_Decorator::~FFloor_Decorator()
{
}

void FFloor_Decorator::Entry()
{
	Super::Entry();
}

void FFloor_Decorator::Quit()
{
	USceneInteractionWorldSystem::GetInstance()->ClearFocus();
	USceneInteractionWorldSystem::GetInstance()->ClearRouteMarker();

	Super::Quit();
}

bool FFloor_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	Super::Operation(OperatorType);

	switch (OperatorType)
	{
	case EOperatorType::kLeftMouseButton:
	case EOperatorType::kRightMouseButton:
		{
			TArray<struct FHitResult> OutHits;

			auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));

			FVector2D MousePosition;
			PCPtr->GetMousePosition(
			                        MousePosition.X,
			                        MousePosition.Y
			                       );

			FVector WorldLocation;
			FVector WorldDirection;
			PCPtr->DeprojectScreenPositionToWorld(
			                                      MousePosition.X,
			                                      MousePosition.Y,
			                                      WorldLocation,
			                                      WorldDirection
			                                     );

			// 优先检测设备
			{
				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(Device_Object);
				GetWorldImp()->LineTraceMultiByObjectType(
				                                          OutHits,
				                                          WorldLocation,
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
				                                                           ->LinetraceDistance),
				                                          ObjectQueryParams
				                                         );

				for (const auto& Iter : OutHits)
				{
					if (Iter.GetActor())
					{
						if (Iter.GetActor()->IsHidden())
						{
							continue;
						}

						USceneInteractionWorldSystem::GetInstance()->ClearFocus();
						USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(Iter.GetActor(), EInteractionType::kView);


						auto MessageBodySPtr = MakeShared<FMessageBody_SelectedDevice>();

						MessageBodySPtr->DeviceID = TEXT("");

						UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);

						return true;
					}
				}
			}

			// 检测区域
			{
				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(Space_Object);
				GetWorldImp()->LineTraceMultiByObjectType(
				                                          OutHits,
				                                          WorldLocation,
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
				                                                           ->LinetraceDistance),
				                                          ObjectQueryParams
				                                         );

				for (const auto& Iter : OutHits)
				{
					if (Iter.GetActor())
					{
						USceneInteractionWorldSystem::GetInstance()->ClearFocus();
						USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(Iter.GetActor(), EInteractionType::kView);

						PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformLocaterBySpace>(
							 [&Iter](
							 UGT_CameraTransformLocaterBySpace* GTPtr
							 )
							 {
								 if (GTPtr)
								 {
									 GTPtr->SpaceActorPtr = Iter.GetActor();
								 }
							 }
							);

						auto MessageBodySPtr = MakeShared<FMessageBody_SelectedSpace>();

						MessageBodySPtr->SpaceName = TEXT("");

						UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
						return true;
					}
				}
			}
		}
		break;
	case EOperatorType::kNone:
		break;
	default: ;
	}

	USceneInteractionWorldSystem::GetInstance()->ClearFocus();

	return false;
}

void FFloor_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);

	auto Result = UKismetAlgorithm::GetCameraSeat(
	                                              InActors,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.ViewRot,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.FOV
	                                             );

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ModifyCameraTransform>(
		 [Result](
		 UGT_ModifyCameraTransform* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->TargetLocation = Result.Key.GetLocation();
				 GTPtr->TargetRotation = Result.Key.GetRotation().Rotator();
				 GTPtr->TargetTargetArmLength = Result.Value;
			 }
		 }
		);
}
