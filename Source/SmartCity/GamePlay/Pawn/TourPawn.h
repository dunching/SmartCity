// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"

#include "TourPawn.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;

/**
 * 漫游Pawn
 */
UCLASS()
class SMARTCITY_API ATourPawn :
	public APawn
{
	GENERATED_BODY()

public:

	ATourPawn(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void PossessedBy(
		AController* NewController
		) override;

	virtual void SetupPlayerInputComponent(
		UInputComponent* PlayerInputComponent
		) override;

	void LerpToSeat(const FTransform& Transform, float SpringArmLen);
	
	void UpdateControlParam(const FControlParam&ControlParam);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerComponent> PlayerComponentPtr = nullptr;
	
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> CollisionComponent;
	
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> MovementComponent;

};
