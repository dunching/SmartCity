// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Components/StateTreeComponent.h"

#include "GenerateTypes.h"

#include "PlayerComponent.generated.h"

struct FInputActionValue;

class USceneComponent;
class UInputActions;
class AHumanCharacter_Player;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class SMARTCITY_API UPlayerComponent :
	public UActorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_Player;

};
