// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "ReplaceActorBase.generated.h"

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
class BUILDINGTOOLS_API AReplaceActorBase :
	public AActor
{
	GENERATED_BODY()

public:

	AReplaceActorBase(
		const FObjectInitializer& ObjectInitializer
		);

};
