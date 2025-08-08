// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "NavagationPaths.generated.h"

class USplineComponent;
class USceneComponent;

USTRUCT(BlueprintType, Blueprintable)
struct EFFECTS_API FMarksHelper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<USceneComponent*, float>LenMap;
};

/**
 * 门禁
 */
UCLASS()
class EFFECTS_API ANavagationPaths :
	public AActor
{
	GENERATED_BODY()

public:
	ANavagationPaths(
		const FObjectInitializer& ObjectInitializer
		);

	void SwitchDisplay(bool bIsDisplay);

protected:

	void UpdateMarks();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> SplineComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UStaticMesh> StaticMesh = nullptr;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TMap<USplineComponent*, FMarksHelper>StaticMeshComponentsAry;

	FTimerHandle TimerHandle;

	float Frequence = 1.f / 30;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform RelativeTransform = FTransform::Identity;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Speed = 300.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Interval = 300.f;
	
};
