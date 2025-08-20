// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "GenerateTypes.h"
#include "SceneElementBase.h"

#include "SceneElement_PWR_Pipe.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UMaterialInterface;
class UStaticMeshComponent;

/**
 * 管道效果
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

	virtual void BeginPlay() override;
	
	virtual void ReplaceImp(
		AActor* ActorPtr
		) override;

	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

protected:

	float EnergyValue = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

	UPROPERTY()
	TMap<UStaticMeshComponent*, FMaterialsCache>OriginalMaterials;

};
