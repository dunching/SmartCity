// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.h"

#include "AIComponent.generated.h"

class USceneComponent;
class ACharacterBase;
class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UDA_NPCAllocation;
class UTaskNode_Temporary;
class AHumanCharacter_AI;
class AGeneratorNPCs_Patrol;
class ABuildingArea;
class UTaskPromt;

/**
 *
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SMARTCITY_API UAIComponent :
	public UActorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_AI;

};
