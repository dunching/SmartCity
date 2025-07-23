#include "SceneInteractionDecorator.h"

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

inline FDecoratorBase::~FDecoratorBase()
{
}

void FDecoratorBase::Entry()
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

FSplitFloorMode_Decorator::FSplitFloorMode_Decorator():
                                                      Super(
                                                            UGameplayTagsLibrary::Interaction_Mode,
                                                            UGameplayTagsLibrary::Interaction_Mode_Tour
                                                           )
{
}

void FSplitFloorMode_Decorator::Entry()
{
	Super::Entry();
}

bool FSplitFloorMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
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

FRadarMode_Decorator::FRadarMode_Decorator():
                                            Super(
                                                  UGameplayTagsLibrary::Interaction_Mode,
                                                  UGameplayTagsLibrary::Interaction_Mode_Radar
                                                 )
{
}

FRadarMode_Decorator::~FRadarMode_Decorator()
{
	GetWorldImp()->GetTimerManager().ClearTimer(
	                                            QueryTimerHadnle
	                                           );
}

void FRadarMode_Decorator::Entry()
{
	Super::Entry();

	GetWorldImp()->GetTimerManager().SetTimer(
	                                          QueryTimerHadnle,
	                                          std::bind(&ThisClass::RadarQuery, this),
	                                          UGameOptions::GetInstance()->RadarQueryFrequency,
	                                          true
	                                         );
}

bool FRadarMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

void FRadarMode_Decorator::RadarQuery()
{
}

FQDMode_Decorator::FQDMode_Decorator():
                                      Super(
                                            UGameplayTagsLibrary::Interaction_Mode,
                                            UGameplayTagsLibrary::Interaction_Mode_QD
                                           )
{
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

	TSet<FSceneActorConditional, TSceneActorConditionalKeyFuncs> FilterTags;

	{
		FSceneActorConditional SceneActorConditional;

		SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

		FilterTags.Add(SceneActorConditional);
	}
	{
		FSceneActorConditional SceneActorConditional;

		SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
		SceneActorConditional.ConditionalSet.AppendTags(USceneInteractionWorldSystem::GetInstance()->GetAllFilterTags());

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

void FArea_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	TSet<FSceneActorConditional, TSceneActorConditionalKeyFuncs> FilterTags;

	{
		FSceneActorConditional SceneActorConditional;

		SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

		FilterTags.Add(SceneActorConditional);
	}
	{
		FSceneActorConditional SceneActorConditional;

		SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
		SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

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

	SmartCityCommand::ReplyCameraTransform();
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

FFloor_Decorator::FFloor_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       Interaction_Area
	      )
{
}

void FFloor_Decorator::Entry()
{
	Super::Entry();
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

			auto PCPtr = GEngine->GetFirstLocalPlayerController(GetWorldImp());

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
						
						ClearFocus();
						AddFocusDevice(Iter.GetActor());

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

	ClearFocus();

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

void FFloor_Decorator::AddFocusDevice(
	AActor* DevicePtr
	)
{
	if (!DevicePtr)
	{
		return;
	}
	if (FocusActors.Contains(DevicePtr))
	{
		return;
	}

	FocusActors.Add(DevicePtr);

	auto PrimitiveComponentPtr = DevicePtr->GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(true);
		PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}
}

void FFloor_Decorator::RemoveFocusDevice(
	AActor* DevicePtr
	)
{
	if (!DevicePtr)
	{
		return;
	}
	if (!FocusActors.Contains(DevicePtr))
	{
		return;
	}

	auto PrimitiveComponentPtr = DevicePtr->GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(false);
	}

	FocusActors.Remove(DevicePtr);
}

void FFloor_Decorator::ClearFocus()
{
	for (auto Iter : FocusActors)
	{
		if (Iter)
		{
			auto PrimitiveComponentPtr = Iter->GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(false);
			}
		}
	}
	FocusActors.Empty();
}
