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
		EDecoratorType OperatorType,
		const TSet<FGameplayTag>& FilterTags,
		const std::function<void(bool,const TSet<AActor*>&)>& OnEnd = nullptr
		);

	void InitializeSceneActors();

private:
	/**
	 * 
	 */
	TMap<EDecoratorType, TSharedPtr<FDecoratorBase>> DecoratorLayerAssetMap;

	/**
	 * 每个装饰器下的过滤条件
	 */
	TMap<EDecoratorType, TSet<FGameplayTag>> Filters;

	/**
	 * 我们在这里存场景里面对象的引用，用于各种视图切换时显示隐藏
	 * 为什么不用DataLayer？因为DataLayer在切换时太慢了
	 * Key 对象引用
	 * Value 仅在这些过滤条件下显示
	 */
	TMap<TWeakObjectPtr<AActor>, TSet<FGameplayTag>> SceneActorsRefMap;

	TMap<FGuid, TWeakObjectPtr<AActor>> ItemRefMap;
};
