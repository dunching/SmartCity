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
		const TSet<FGameplayTag>& FilterTags,
		const std::function<void(bool,const TSet<AActor*>&)>& OnEnd = nullptr
		);

	void InitializeSceneActors();

	TWeakObjectPtr<AActor>FindSceneActor(const FGuid& ID)const;
	
private:
	/**
	 * 
	 */
	TMap<FGameplayTag, TSharedPtr<FDecoratorBase>> DecoratorLayerAssetMap;

	TMap<FGuid, TWeakObjectPtr<AActor>> ItemRefMap;
};
