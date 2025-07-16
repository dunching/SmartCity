// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include <AIController.h>

#include "HumanCharacter_AI.h"
#include "PlanetControllerInterface.h"

#include "PlanetPlayerController.generated.h"

/**
 *
 */
UCLASS()
class SMARTCITY_API APlanetPlayerController :
	public APlayerController,
	public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

};
