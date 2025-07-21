#include "SceneInteractionWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Kismet/GameplayStatics.h"

#include "Tools.h"
#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "GameplayTagsLibrary.h"
#include "LogWriter.h"
#include "SceneInteractionDecorator.h"
#include "Engine/Light.h"
#include "Engine/StaticMeshActor.h"

USceneInteractionWorldSystem* USceneInteractionWorldSystem::GetInstance()
{
	return Cast<USceneInteractionWorldSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(
			GetWorldImp(),
			USceneInteractionWorldSystem::StaticClass()
		)
	);
}

void USceneInteractionWorldSystem::SwitchInteractionMode(const FGameplayTag& Interaction_Mode)
{
	if (Interaction_Mode == UGameplayTagsLibrary::Interaction_Mode_Scene)
	{
	}
}

void USceneInteractionWorldSystem::SwitchViewArea(const FGameplayTag& Interaction_Area)
{
	if (Interaction_Area.MatchesTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall))
	{
		if (DecoratorLayerAssetMap.Contains(EDecoratorType::kArea))
		{
			if (DecoratorLayerAssetMap[EDecoratorType::kArea]->GetBranchDecoratorType() ==
				EDecoratorType::kArea_ExternalWall)
			{
				return;
			}
		}

		auto DecoratorSPtr = MakeShared<FExternalWall_Decorator>(Interaction_Area);
		DecoratorSPtr->Entry();

		DecoratorLayerAssetMap.Add(EDecoratorType::kArea,
		                           DecoratorSPtr);
		return;
	}

	if (Interaction_Area.MatchesTag(UGameplayTagsLibrary::Interaction_Area_Floor))
	{
		if (DecoratorLayerAssetMap.Contains(EDecoratorType::kArea))
		{
			if (DecoratorLayerAssetMap[EDecoratorType::kArea]->GetBranchDecoratorType() ==
				EDecoratorType::kArea_Floor)
			{
				return;
			}
		}

		auto DecoratorSPtr = MakeShared<FFloor_Decorator>(Interaction_Area);
		DecoratorSPtr->Entry();

		DecoratorLayerAssetMap.Add(EDecoratorType::kArea,
		                           DecoratorSPtr);
		return;
	}
}

void USceneInteractionWorldSystem::Operation(EOperatorType OperatorType) const
{
	for (const auto& Iter : DecoratorLayerAssetMap)
	{
		if (Iter.Value)
		{
			Iter.Value->Operation(OperatorType);
		}
	}
}

TSet<AActor*> USceneInteractionWorldSystem::UpdateFilter(EDecoratorType DecoratorType,
                                                const TSet<FGameplayTag>& FilterTags)
{
	TSet<AActor*>Result;
	
	// 更新过滤条件
	Filters.Add(DecoratorType,
	            FilterTags);

	TArray<AActor*> ResultAry;
	UGameplayStatics::GetAllActorsOfClass(this,
	                                      AActor::StaticClass(),
	                                      ResultAry);

	auto Lambda = [this, &Result](AActor* Actor)
	{
		if (SceneActorsRefMap.Contains(Actor))
		{
		}
		else
		{
			PRINTINVOKEWITHSTR(FString(TEXT("")));
			return;
		}

		if (Actor->IsA(ALight::StaticClass()))
		{
			PRINTINVOKEWITHSTR(FString(TEXT("")));
			Actor->SetActorHiddenInGame(true);
			return;
		}
		
		auto Filter = SceneActorsRefMap[Actor];

		TSet<FGameplayTag>FilterSet;
		for (const auto& Iter : Filters)
		{
			FilterSet.Append(Iter.Value);
		}

		for (const auto& Iter : FilterSet)
		{
			if (Filter.Contains(Iter))
			{
				
			}
			else
			{
				PRINTINVOKEWITHSTR(FString(TEXT("")));
				Actor->SetActorHiddenInGame(true);
				return;
			}
		}
		
		PRINTINVOKEWITHSTR(FString(TEXT("")));
		Actor->SetActorHiddenInGame(false);

		Result.Add(Actor);
	};

	for (auto Iter : ResultAry)
	{
		if (Iter)
		{
			Lambda(Iter);
		}
	}

	return Result;
}

void USceneInteractionWorldSystem::InitializeSceneActors()
{
	TArray<AActor*> ResultAry;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(),
										  AActor::StaticClass(),
										  ResultAry);

	for (auto Iter : ResultAry)
	{
		if (Iter->IsA(AStaticMeshActor::StaticClass()))
		{
			
		}
		else if (!Iter->GetComponents().IsEmpty())
		{
			
		}
		else
		{
			continue;
		}
		
		if (Iter->ActorHasTag(TEXT("All")))
		{
			SceneActorsRefMap.Add(Iter, {UGameplayTagsLibrary::Interaction_Area_ExternalWall});
		}
		else if (Iter->ActorHasTag(TEXT("F1")))
		{
			SceneActorsRefMap.Add(Iter, {UGameplayTagsLibrary::Interaction_Area_Floor_F1});
		}
	}
}
