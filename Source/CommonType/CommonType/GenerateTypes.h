// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "GenerateTypes.generated.h"

class ADatasmithSceneActor;

UENUM()
enum class EOperatorType: uint8
{
	kLeftMouseButton,

	kRightMouseButton,

	kNone,
};

USTRUCT(BlueprintType, Blueprintable)
struct FSceneActorMap
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<TSoftObjectPtr<ADatasmithSceneActor>> DataSmithSceneActorsSet;
	
};
