// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "FloorHelperBase.h"

#include "FloorHelper.generated.h"

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
class SMARTCITY_API AFloorHelper :
	public AFloorHelperBase
{
	GENERATED_BODY()

public:

};

UCLASS()
class SMARTCITY_API ABuilding_Floor_Mask :
	public AActor
{
	GENERATED_BODY()

public:
	ABuilding_Floor_Mask(
		const FObjectInitializer& ObjectInitializer
		);

	void SetFloor(AFloorHelper*FloorPtr);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

};
