#include "SceneInteractionDecorator.h"

#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

#include "AssetRefMap.h"
#include "LogWriter.h"

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

void FDecoratorBase::Operation() const
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

void FTourDecorator::Operation() const
{
	Super::Operation();

	PRINTFUNCSTR();
}

FSceneMode_Decorator::FSceneMode_Decorator():
	Super(EDecoratorType::kMode,
	      EDecoratorType::kMode_Scene)
{
}

void FSceneMode_Decorator::Operation() const
{
	Super::Operation();

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
					EDataLayerRuntimeState::Loaded
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

void FExternalWall_Decorator::Operation() const
{
	Super::Operation();

	PRINTFUNCSTR();
}

FFloor_Decorator::FFloor_Decorator(
	const FGameplayTag& Interaction_Area):
	Super(EDecoratorType::kArea_Floor,
	      Interaction_Area)
{
}

void FFloor_Decorator::Operation() const
{
	Super::Operation();
}
