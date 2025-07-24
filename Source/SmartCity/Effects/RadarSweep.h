// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"

#include "RadarSweep.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;

/**
 * 雷达扫描效果··
 */
UCLASS()
class SMARTCITY_API ARadarSweep :
	public AActor
{
	GENERATED_BODY()

public:

	ARadarSweep(
		const FObjectInitializer& ObjectInitializer
		);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
	
};
