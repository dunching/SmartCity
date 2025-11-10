// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "BuildingGenerateTypes.h"

#include "SceneElementCategory.generated.h"

class USceneComponent;

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ASceneElementCategory :
	public AActor
{
	GENERATED_BODY()

public:
	ASceneElementCategory(
		const FObjectInitializer& ObjectInitializer
		);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameplayTag Category;
};
