// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_InfraredDetector.generated.h"

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
class SMARTCITY_API ASceneElement_InfraredDetector :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:

	ASceneElement_InfraredDetector(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual FBox GetComponentsBoundingBox(
		bool bNonColliding = false,
		bool bIncludeFromChildActors = false
		) const override;

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> AnchorComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SweepEffectStaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 Deepth = 120;
	
private:
};
