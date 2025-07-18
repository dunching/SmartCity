// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"

#include "ViewerPawn.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;

/**
 * 机位
 */
UCLASS()
class SMARTCITY_API AViewerPawn :
	public APawn
{
	GENERATED_BODY()

public:

	AViewerPawn(
		const FObjectInitializer& ObjectInitializer
		);

	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(Category = Pawn, EditInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayTag SeatTag;
	
};
