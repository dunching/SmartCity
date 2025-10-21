// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_Space_PolygonHelper.generated.h"

class USplineComponent;
class USplineMeshComponent;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_Space_PolygonHelper :
	public AActor
{
	GENERATED_BODY()

public:
	ASceneElement_Space_PolygonHelper(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USplineComponent>SplineComponentPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UStaticMesh>StaticMeshRef = nullptr;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly
	)
	TSoftObjectPtr<UMaterialInstance> MaterialRef;
	
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly
	)
	TArray<USplineMeshComponent*> SplineMeshComponentsAry;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly
	)
	FVector2D Scale = FVector2D(.2f,.2f);

};
