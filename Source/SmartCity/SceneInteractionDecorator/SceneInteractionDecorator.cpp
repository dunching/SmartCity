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

FDecoratorBase::FDecoratorBase(EDecoratorType InMainDecoratorType,
                               EDecoratorType InBranchDecoratorType): MainDecoratorType(InMainDecoratorType),
                                                                      BranchDecoratorType(InBranchDecoratorType)
{
}

inline FDecoratorBase::~FDecoratorBase()
{
}

void FDecoratorBase::Entry()
{
}

void FDecoratorBase::Operation(EOperatorType OperatorType) const
{
}

EDecoratorType FDecoratorBase::GetMainDecoratorType() const
{
	return MainDecoratorType;
}

EDecoratorType FDecoratorBase::GetBranchDecoratorType() const
{
	return BranchDecoratorType;
}

void FTourDecorator::Entry()
{
	Super::Entry();

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(GetMainDecoratorType(),
	                                                          {});
}

FTourDecorator::FTourDecorator():
	Super(EDecoratorType::kMode,
	      EDecoratorType::kMode_Tour)
{
}

void FTourDecorator::Operation(EOperatorType OperatorType) const
{
	Super::Operation(OperatorType);

	PRINTFUNCSTR();
}

FSceneMode_Decorator::FSceneMode_Decorator():
	Super(EDecoratorType::kMode,
	      EDecoratorType::kMode_Scene)
{
}

void FSceneMode_Decorator::Entry()
{
	Super::Entry();

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(GetMainDecoratorType(),
	                                                          {});
}

void FSceneMode_Decorator::Operation(EOperatorType OperatorType) const
{
	Super::Operation(OperatorType);

	PRINTFUNCSTR();
}

FArea_Decorator::FArea_Decorator(EDecoratorType InBranchDecoratorType,
                                 const FGameplayTag& Interaction_Area):
	Super(EDecoratorType::kArea,
	      InBranchDecoratorType),
	CurrentInteraction_Area(Interaction_Area)
{
}

void FArea_Decorator::Entry()
{
	Super::Entry();

	TSet<TSoftObjectPtr<UDataLayerAsset>> DalaLayerAssetMap;

	Actors = USceneInteractionWorldSystem::GetInstance()->UpdateFilter(GetMainDecoratorType(),
	                                                                   {CurrentInteraction_Area});
}

FExternalWall_Decorator::FExternalWall_Decorator(
	const FGameplayTag& Interaction_Area):
	Super(EDecoratorType::kArea_ExternalWall,
	      Interaction_Area)
{
}

void FExternalWall_Decorator::Entry()
{
	Super::Entry();

	SmartCityCommand::ReplyCameraTransform();
}

void FExternalWall_Decorator::Operation(EOperatorType OperatorType) const
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
}

FFloor_Decorator::FFloor_Decorator(
	const FGameplayTag& Interaction_Area):
	Super(EDecoratorType::kArea_Floor,
	      Interaction_Area)
{
}

void FFloor_Decorator::Entry()
{
	Super::Entry();

	auto Result = UKismetAlgorithm::GetCameraSeat(Actors,
	                                UGameOptions::GetInstance()->ViewFloorRot,
	                                UGameOptions::GetInstance()->ViewFloorFOV);
	
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ModifyCameraTransform>([Result](UGT_ModifyCameraTransform* GTPtr)
	{
		if (GTPtr)
		{
			GTPtr->TargetLocation = Result.Key.GetLocation();
			GTPtr->TargetRotation = Result.Key.GetRotation().Rotator();
			GTPtr->TargetTargetArmLength = Result.Value;
		}
	});
}

void FFloor_Decorator::Operation(EOperatorType OperatorType) const
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
			PCPtr->GetMousePosition(MousePosition.X,
			                        MousePosition.Y);

			FVector WorldLocation;
			FVector WorldDirection;
			PCPtr->DeprojectScreenPositionToWorld(MousePosition.X,
			                                      MousePosition.Y,
			                                      WorldLocation,
			                                      WorldDirection);

			// 优先检测设备
			{
				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(Device_Object);
				GetWorldImp()->LineTraceMultiByObjectType(OutHits,
				                                          WorldLocation,
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
					                                          ->LinetraceDistance),
				                                          ObjectQueryParams);

				for (const auto& Iter : OutHits)
				{
					if (Iter.GetActor())
					{
						auto MessageBodySPtr = MakeShared<FMessageBody_SelectedDevice>();

						MessageBodySPtr->DeviceID = TEXT("");

						UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);

						return;
					}
				}
			}

			// 检测区域
			{
				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(Device_Object);
				GetWorldImp()->LineTraceMultiByObjectType(OutHits,
				                                          WorldLocation,
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
					                                          ->LinetraceDistance),
				                                          ObjectQueryParams);

				for (const auto& Iter : OutHits)
				{
					if (Iter.GetActor())
					{
						auto MessageBodySPtr = MakeShared<FMessageBody_SelectedSpace>();

						MessageBodySPtr->SpaceName = TEXT("");

						UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
						return;
					}
				}
			}
		}
		break;
	case EOperatorType::kNone:
		break;
	default: ;
	}
}
