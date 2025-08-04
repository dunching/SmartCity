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

	
	static FGameplayTag Interaction_Area;

	static FGameplayTag Interaction_Area_ExternalWall;

	static FGameplayTag Interaction_Area_Floor;

	static FGameplayTag Interaction_Area_Floor_F1;

	static FGameplayTag Interaction_Area_Space;

	static FGameplayTag Interaction_Area_SplitFloor;

	
	static FGameplayTag Interaction_Mode;

	static FGameplayTag Interaction_Mode_Empty;

	static FGameplayTag Interaction_Mode_Tour;

	static FGameplayTag Interaction_Mode_Scene;

	static FGameplayTag Interaction_Mode_Radar;

	static FGameplayTag Interaction_Mode_QD;

	static FGameplayTag Interaction_Mode_RD;

	static FGameplayTag Interaction_Mode_AccessControl;

	static FGameplayTag Interaction_Mode_Lighting;

	static FGameplayTag Interaction_Mode_HVAC;

	static FGameplayTag Interaction_Mode_SunShade;

	static FGameplayTag Interaction_Mode_Monitor;

	static FGameplayTag Interaction_Mode_Elevator;

	static FGameplayTag Interaction_Mode_Intelligence;

	static FGameplayTag Interaction_Mode_Energy;
	

	static FGameplayTag SceneElement_FanCoil;

};