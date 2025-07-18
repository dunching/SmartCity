// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LimitAreaSplineHelper.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

class APawn;
class AFloatPawn;

/**
 *
 */
UCLASS()
class UTILS_API ALimitAreaSplineHelper : public AActor
{
	GENERATED_BODY()
public:

	ALimitAreaSplineHelper(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay()override;

	virtual void Tick(float DeltaSeconds)override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USplineComponent* SplineComponentPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<USplineMeshComponent* > SplineMeshAry;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSoftObjectPtr<UStaticMesh>SplineMesh;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSoftObjectPtr<UMaterialInstance>limitMat;

	UPROPERTY(Category = Pawn, BlueprintReadOnly, EditDefaultsOnly)
		float Interaval = 500.f * 100.f;

	UPROPERTY(Category = Pawn, BlueprintReadOnly, EditDefaultsOnly)
		float DeltaTime = 0.1f;

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<UMaterialInstanceDynamic*>MatInstAry;

	APawn* PawnPtr = nullptr;

	float CurrentDeltaTime = 0.f;

};
