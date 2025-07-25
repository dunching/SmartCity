// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"

#include "Elevator.generated.h"

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
class SMARTCITY_API AElevator :
	public AActor
{
	GENERATED_BODY()

public:

	AElevator(
		const FObjectInitializer& ObjectInitializer
		);

	virtual  void Tick(float DeltaSeconds) override;
	
};
