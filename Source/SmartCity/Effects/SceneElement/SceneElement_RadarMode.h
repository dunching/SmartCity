// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_RadarMode.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class APersonMark;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_RadarMode :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:
	ASceneElement_RadarMode(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void BeginPlay() override;
	
	virtual void Tick(
		float DeltaTime
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent>StaticMeshComponent;

private:
};
