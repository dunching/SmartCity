// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_DeviceBase.generated.h"

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
class SMARTCITY_API ASceneElement_DeviceBase :
	public ASceneElementBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void ReplaceImp(
		AActor* ActorPtr
		) override;

	virtual void BeginInteraction() override;

	virtual void EndInteraction() override;

	virtual TMap<FString,FString> GetStateDescription()const override;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere )
	FGameplayTag DeviceType ;

	UPROPERTY(BlueprintReadWrite,EditAnywhere )
	FString  DeviceID ;

	bool bIsOpened = false;
};
