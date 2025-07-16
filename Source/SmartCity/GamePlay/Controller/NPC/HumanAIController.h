// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "PlanetAIController.h"

#include "HumanAIController.generated.h"

class USplineComponent;
class UCharacterTitle;
class UTeamMatesHelperComponent;
struct FCharacterProxy;
class ACharacterBase;
class AHumanCharacter_AI;
class UStateTreeComponent;
class UAIControllerStateTreeAIComponent;
class UAIPerceptionComponent;
class ABuildingArea;
class AGeneratorColony_ByTime;
class AGeneratorNPCs_Patrol;
class AGroupManagger;

/**
 *
 */
UCLASS()
class SMARTCITY_API AHumanAIController :
	public APlanetAIController
{
	GENERATED_BODY()

public:
};
