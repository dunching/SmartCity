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

FDecoratorBase::FDecoratorBase(EDecoratorType InMainDecoratorType,
                               EDecoratorType InBranchDecoratorType): MainDecoratorType(InMainDecoratorType),
                                                                      BranchDecoratorType(InBranchDecoratorType)
{
}

inline FDecoratorBase::~FDecoratorBase()
{
}

void FDecoratorBase::Entry() const
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

void FArea_Decorator::Entry() const
{
	Super::Entry();

	TSet<TSoftObjectPtr<UDataLayerAsset>> DalaLayerAssetMap;

	if (UAssetRefMap::GetInstance()->DataLayerAssetMap.Contains(CurrentInteraction_Area))
	{
		DalaLayerAssetMap.Add(
			UAssetRefMap::GetInstance()->DataLayerAssetMap[CurrentInteraction_Area]);
	}

	SwitchViewArea(DalaLayerAssetMap);
}

void FArea_Decorator::SwitchViewArea(const TSet<TSoftObjectPtr<UDataLayerAsset>>& DalaLayerAssetSet) const
{
	auto DataLayerManagerPtr = UDataLayerManager::GetDataLayerManager(GetWorldImp());

	const auto DataLayerAssetMap = UAssetRefMap::GetInstance()->DataLayerAssetMap;
	for (auto Iter : DataLayerAssetMap)
	{
		// 要显示
		if (DalaLayerAssetSet.Contains(Iter.Value))
		{
			if (
				DataLayerManagerPtr->
				SetDataLayerRuntimeState(
					Iter.Value.LoadSynchronous(),
					EDataLayerRuntimeState::Activated
				))
			{
				PRINTINVOKEWITHSTR(FString(TEXT("Activated")));
			}
		}
		else
		{
			if (
				DataLayerManagerPtr->
				SetDataLayerRuntimeState(
					Iter.Value.LoadSynchronous(),
					EDataLayerRuntimeState::Unloaded
				))
			{
				PRINTINVOKEWITHSTR(FString(TEXT("Unload")));
			}
		}
	}
}

FExternalWall_Decorator::FExternalWall_Decorator(
	const FGameplayTag& Interaction_Area):
	Super(EDecoratorType::kArea_ExternalWall,
	      Interaction_Area)
{
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
