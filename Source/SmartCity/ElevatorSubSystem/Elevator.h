// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

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
	public ASceneElementBase
{
	GENERATED_BODY()

public:

	AElevator(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	void ChangeTargetFloorIndex(int32 FloorIndex); 
	
protected:

	UFUNCTION()
	void MoveElevator();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MoveSpeed = 100;
	
	int32 TargetFloorIndex = 0;

	FVector TargetFloorLocation = FVector::ZeroVector;

	bool bIsUp = false;
	
	FTimerHandle MoveTimerHandle;

	float Frequence = 1.f / 30;
};
