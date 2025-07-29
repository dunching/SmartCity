// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"

#include "SceneElementBase.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElementBase :
	public AActor
{
	GENERATED_BODY()

public:

	ASceneElementBase(
		const FObjectInitializer& ObjectInitializer
		);

};
