// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "AIController.h"
#include "PlanetControllerInterface.h"

#include "PlanetAIController.generated.h"

class UPlanetAbilitySystemComponent;
struct FCharacterProxy;
class ACharacterBase;
class UCharacterAttributesComponent;
class UInventoryComponent;
class UTalentAllocationComponent;
class UTeamMatesHelperComponent;
class AGroupManagger;
class USplineComponent;


UCLASS()
class SMARTCITY_API APlanetAIController :
	public AAIController,
	public IPlanetControllerInterface
{
	GENERATED_BODY()

public:
	using FPawnType = ACharacterBase;

};
