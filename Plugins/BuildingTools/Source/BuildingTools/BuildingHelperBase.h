// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"

#include "BuildingGenerateTypes.h"

#include "BuildingHelperBase.generated.h"

class ANavagationPaths;
class UBoxComponent;

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ABuildingHelperBase :
	public AActor
{
	GENERATED_BODY()

public:

	ABuildingHelperBase(
		const FObjectInitializer& ObjectInitializer
		);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UBoxComponent> BoxComponentPtr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag FloorTag;
	
	/**
	 * 该楼宇的所有引用
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSceneElementMap AllReference;

};
