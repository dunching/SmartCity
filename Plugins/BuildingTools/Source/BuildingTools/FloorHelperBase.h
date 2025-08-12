// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"

#include "BuildingGenerateTypes.h"

#include "FloorHelperBase.generated.h"

class ANavagationPaths;
class UBoxComponent;

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API AFloorHelperBase :
	public AActor
{
	GENERATED_BODY()

public:

	AFloorHelperBase(
		const FObjectInitializer& ObjectInitializer
		);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UBoxComponent> BoxComponentPtr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<ANavagationPaths> NavagationPaths = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag FloorTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 FloorIndex = 0;
	
	/**
	 * 该层的所有引用
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSceneElementMap AllReference;

};
