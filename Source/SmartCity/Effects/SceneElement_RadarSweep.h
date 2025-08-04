// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_RadarSweep.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_RadarSweep :
	public ASceneElementBase
{
	GENERATED_BODY()

public:

	ASceneElement_RadarSweep(
		const FObjectInitializer& ObjectInitializer
		);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SweepEffectStaticMeshComponent = nullptr;

	int32 Horizontal = 120;
	
	int32 Vertical = 90;
	
};
