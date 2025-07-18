// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Subsystems/GameInstanceSubsystem.h>
#include <Subsystems/EngineSubsystem.h>

#include "GameplayTagsLibrary.generated.h"

UCLASS()
class GAMEPLAYTAGSLIBRARY_API UGameplayTagsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static FGameplayTag Seat_Default;

	static FGameplayTag Interaction_Mode;

	static FGameplayTag Interaction_Mode_Scene;

	static FGameplayTag Interaction_Area;

	static FGameplayTag Interaction_Area_ExternalWall;

	static FGameplayTag Interaction_Area_Floor;

	static FGameplayTag Interaction_Area_Space;

};