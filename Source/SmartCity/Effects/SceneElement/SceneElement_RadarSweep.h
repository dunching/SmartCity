// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_RadarSweep.generated.h"

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
class SMARTCITY_API ASceneElement_RadarSweep :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:
	ASceneElement_RadarSweep(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void BeginPlay() override;
	
	virtual void Tick(
		float DeltaTime
		) override;

	virtual FBox GetComponentsBoundingBox(
		bool bNonColliding = false,
		bool bIncludeFromChildActors = false
		) const override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent>StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 Deepth = 120;

private:
	void EntryQuery();

	void QuitQuery();

	void RadarQuery();

	void QueryComplete();

	void UpdateMeshEffect();

	FTimerHandle UpdateMeshTimerHandle;

	FTimerHandle QueryTimerHandle;

	TArray<APersonMark*> GeneratedMarkers;
};
