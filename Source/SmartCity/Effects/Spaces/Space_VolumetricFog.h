// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Space_VolumetricFog.generated.h"

class USplineComponent;
class USplineMeshComponent;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASpace_VolumetricFog :
	public AActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetBoxSize(
		const FVector& InBoxSize
		);

	UFUNCTION(BlueprintImplementableEvent)
	void SetFogColor(
		const FColor& FogColor
		);
};
