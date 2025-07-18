// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LimitAreaCubeHelper.generated.h"

class UBoxComponent;
class USplineComponent;
class USplineMeshComponent;
class UMaterialInstanceDynamic;

class AFloatPawn;

/**
 *
 */
UCLASS()
class UTILS_API ALimitAreaCubeHelper : public AActor
{
	GENERATED_BODY()
public:

	ALimitAreaCubeHelper(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay()override;

	virtual void Tick(float DeltaSeconds)override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USceneComponent* SceneComponentPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBoxComponent* TopBoxComponentPtr = nullptr;
		
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBoxComponent* BottomBoxComponentPtr = nullptr;
		
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBoxComponent* LeftBoxComponentPtr = nullptr;
		
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBoxComponent* RightBoxComponentPtr = nullptr;
		
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBoxComponent* ForwardBoxComponentPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBoxComponent* BackwardBoxComponentPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FVector Size = FVector(1000);

protected:



};
