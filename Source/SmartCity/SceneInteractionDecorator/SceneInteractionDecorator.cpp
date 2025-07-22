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
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"

FDecoratorBase::FDecoratorBase(
	EDecoratorType InMainDecoratorType,
	EDecoratorType InBranchDecoratorType
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

bool FDecoratorBase::Operation(
	EOperatorType OperatorType
	)
{
	return false;
}

EDecoratorType FDecoratorBase::GetMainDecoratorType() const
{
	return MainDecoratorType;
}

EDecoratorType FDecoratorBase::GetBranchDecoratorType() const
{
	return BranchDecoratorType;
}

void FTour_Decorator::Entry()
{
	Super::Entry();

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          GetMainDecoratorType(),
	                                                          {}
	                                                         );
}

FTour_Decorator::FTour_Decorator():
                                  Super(
                                        EDecoratorType::kMode,
                                        EDecoratorType::kMode_Tour
                                       )
{
}

bool FTour_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	PRINTFUNCSTR();
	
	return  Super::Operation(OperatorType);
}

FSceneMode_Decorator::FSceneMode_Decorator():
                                            Super(
                                                  EDecoratorType::kMode,
                                                  EDecoratorType::kMode_Scene
                                                 )
{
}

void FSceneMode_Decorator::Entry()
{
	Super::Entry();

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          GetMainDecoratorType(),
	                                                          {}
	                                                         );
}

bool FSceneMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{

	PRINTFUNCSTR();
	
	return  Super::Operation(OperatorType);
}

FRadarMode_Decorator::FRadarMode_Decorator():
                                            Super(
                                                  EDecoratorType::kMode,
                                                  EDecoratorType::kMode_Radar
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
	return  Super::Operation(OperatorType);
}

void FRadarMode_Decorator::RadarQuery()
{
}

FArea_Decorator::FArea_Decorator(
	EDecoratorType InBranchDecoratorType,
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       EDecoratorType::kArea,
	       InBranchDecoratorType
	      )
	 , CurrentInteraction_Area(Interaction_Area)
{
}

void FArea_Decorator::Entry()
{
	Super::Entry();

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          GetMainDecoratorType(),
	                                                          {CurrentInteraction_Area},
	                                                          std::bind(
	                                                                    &ThisClass::OnUpdateFilterComplete,
	                                                                    this,
	                                                                    std::placeholders::_1,
	                                                                    std::placeholders::_2
	                                                                   )
	                                                         );
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
	       EDecoratorType::kArea_ExternalWall,
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
	       EDecoratorType::kArea_Floor,
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
	                                              UGameOptions::GetInstance()->ViewFloorRot,
	                                              UGameOptions::GetInstance()->ViewFloorFOV
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
