// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_PWR_Pipe.generated.h"

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
class SMARTCITY_API ASceneElement_PWR_Pipe :
	public ASceneElementBase
{
	GENERATED_BODY()

public:
	ASceneElement_PWR_Pipe(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void ReplaceImp(
		AActor* ActorPtr
		) override;

	virtual void SwitchInteractionType(
		EInteractionType InteractionType
		) override;
};
