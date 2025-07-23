// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"

#include "SceneInteractionWorldSystem.generated.h"

class FDecoratorBase;
class UGT_InitializeSceneActors;
class UGT_SceneObjSwitch;

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
		const TSet<FSceneActorConditional, TSceneActorConditionalKeyFuncs>& FilterTags,
		const std::function<void(
			bool,
			const TSet<AActor*>&
			
			)>& OnEnd = nullptr
		);

	void InitializeSceneActors();

	TWeakObjectPtr<AActor> FindSceneActor(
		const FGuid& ID
		) const;

	/**
	 * 根据选择的装饰器获取所有过滤条件
	 * @return 
	 */
	FGameplayTagContainer GetAllFilterTags() const;

private:
	void NotifyOtherDecorators(
		const FGameplayTag& MainTag,
		const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
		) const;

	template <typename Decorator, typename... Args>
	void SwitchDecoratorImp(
		const FGameplayTag& MainTag,
		const FGameplayTag& BranchTag,
		Args... Param
		);

	/**
	 * 
	 */
	TMap<FGameplayTag, TSharedPtr<FDecoratorBase>> DecoratorLayerAssetMap;

	TMap<FGuid, TWeakObjectPtr<AActor>> ItemRefMap;
};

template <typename Decorator, typename... Args>
void USceneInteractionWorldSystem::SwitchDecoratorImp(
	const FGameplayTag& MainTag,
	const FGameplayTag& BranchTag,
	Args... Param
	)
{
	auto DecoratorSPtr = MakeShared<Decorator>(Param...);

	NotifyOtherDecorators(MainTag, DecoratorSPtr);

	DecoratorLayerAssetMap.Add(
	                           MainTag,
	                           DecoratorSPtr
	                          );

	DecoratorSPtr->Entry();
}
