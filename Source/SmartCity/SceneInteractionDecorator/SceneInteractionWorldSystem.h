 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"

#include "SceneInteractionWorldSystem.generated.h"

class FDecoratorBase;

/*
 */
UCLASS()
class SMARTCITY_API USceneInteractionWorldSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static USceneInteractionWorldSystem* GetInstance();

	void SwitchInteractionMode(const FGameplayTag&Interaction_Mode);

	void SwitchViewArea(const FGameplayTag&Interaction_Area);

	void Operation(EOperatorType OperatorType) const;

private:

	TMap<EDecoratorType, TSharedPtr<FDecoratorBase>> DecoratorLayerAssetMap;
	
};