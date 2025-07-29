// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GenerateTypes.h"

#include "SpaceSubSystem.generated.h"

struct FPawnDataStruct;
struct FSceneTool;


class UMainHUDLayout;
class UMainMenuLayout;

/**
 * 
 */
UCLASS()
class SMARTCITY_API USpaceSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static USpaceSubSystem* GetInstance();

	void ElevatorMoveToFloor(const FGameplayTag&ElevatorTag, int32 FloorIndex);
protected:

private:
};
