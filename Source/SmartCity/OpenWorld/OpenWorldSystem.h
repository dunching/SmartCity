 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "OpenWorldSystem.generated.h"

/*
 * 传送
 * Only Server
 */
UCLASS()
class SMARTCITY_API UOpenWorldSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UOpenWorldSubSystem* GetInstance();
private:

	
	
};
