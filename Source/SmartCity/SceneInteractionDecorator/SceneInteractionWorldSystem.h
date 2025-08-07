// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"
#include "SceneElementBase.h"

#include "SceneInteractionWorldSystem.generated.h"

class FDecoratorBase;
class UGT_InitializeSceneActors;
class UGT_SceneObjSwitch;
class URouteMarker;

/*
 * 
 */
UCLASS()
class SMARTCITY_API USceneInteractionWorldSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	friend UGT_InitializeSceneActors;
	friend UGT_SceneObjSwitch;

	static USceneInteractionWorldSystem* GetInstance();

	TSharedPtr<FDecoratorBase> GetInteractionModeDecorator() const;

	TSharedPtr<FDecoratorBase> GetDecorator(const FGameplayTag& Interaction)const;
	
	void SwitchInteractionMode(
		const FGameplayTag& Interaction_Mode
		);

	void SwitchViewArea(
		const FGameplayTag& Interaction_Area
		);

	void Operation(
		EOperatorType OperatorType
		) const;

	void UpdateFilter(
		const FSceneElementConditional& FilterTags,
		const std::function<void(
			bool,
			const TSet<AActor*>&


			
			)>& OnEnd = nullptr
		);

	void InitializeSceneActors();

	TWeakObjectPtr<AActor> FindSceneActor(
		const FGuid& ID
		) const;

	FString GetName(
		AActor* DevicePtr
		) const;

	/**
	 * 根据选择的装饰器获取所有过滤条件
	 * @return 
	 */
	FGameplayTagContainer GetAllFilterTags() const;

	void SwitchInteractionType(
		AActor* DevicePtr,
		const FSceneElementConditional& FilterTags
		);

	void ClearFocus();

	void ClearRouteMarker();

	template <typename Decorator, typename... Args>
	void SwitchDecoratorImp(
		const FGameplayTag& MainTag,
		const FGameplayTag& BranchTag,
		Args... Param
		);

private:
	void NotifyOtherDecoratorsWhenEntry(
		const FGameplayTag& MainTag,
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) const;

	void NotifyOtherDecoratorsWhenQuit(
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) const;

	/**
	 * Key:装饰的类型
	 * Value：当前装饰
	 */
	TMap<FGameplayTag, TSharedPtr<FDecoratorBase>> DecoratorLayerAssetMap;

	TMap<FGuid, TWeakObjectPtr<AActor>> ItemRefMap;

	TSet<AActor*> FocusActors;

	UPROPERTY(Transient)
	TMap<AActor*, URouteMarker*> RouteMarkers;
};

template <typename Decorator, typename... Args>
void USceneInteractionWorldSystem::SwitchDecoratorImp(
	const FGameplayTag& MainTag,
	const FGameplayTag& BranchTag,
	Args... Param
	)
{
	if (DecoratorLayerAssetMap.Contains(MainTag))
	{
		auto OldDecoratorSPtr = DecoratorLayerAssetMap[MainTag];
		if (OldDecoratorSPtr->NeedAsync())
		{
			OldDecoratorSPtr->Quit();
			NotifyOtherDecoratorsWhenQuit(OldDecoratorSPtr);

			OldDecoratorSPtr->OnAsyncQuitComplete.BindLambda(
			                                                 [this, MainTag, Param...]()
			                                                 {
				                                                 auto DecoratorSPtr = MakeShared<Decorator>(Param...);

				                                                 NotifyOtherDecoratorsWhenEntry(MainTag, DecoratorSPtr);

				                                                 DecoratorLayerAssetMap.Add(
					                                                  MainTag,
					                                                  DecoratorSPtr
					                                                 );

			DecoratorSPtr->Entry();
			                                                 }
			                                                );
		}
		else
		{
			auto DecoratorSPtr = MakeShared<Decorator>(Param...);

			NotifyOtherDecoratorsWhenEntry(MainTag, DecoratorSPtr);

			DecoratorLayerAssetMap.Add(
			                           MainTag,
			                           DecoratorSPtr
			                          );

			if (OldDecoratorSPtr)
			{
				OldDecoratorSPtr->Quit();
				NotifyOtherDecoratorsWhenQuit(OldDecoratorSPtr);
			}

			DecoratorSPtr->Entry();
		}
	}
	else
	{
		auto DecoratorSPtr = MakeShared<Decorator>(Param...);

		NotifyOtherDecoratorsWhenEntry(MainTag, DecoratorSPtr);

		DecoratorLayerAssetMap.Add(
		                           MainTag,
		                           DecoratorSPtr
		                          );

		DecoratorSPtr->Entry();
	}
}
