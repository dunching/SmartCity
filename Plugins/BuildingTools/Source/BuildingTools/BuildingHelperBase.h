// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"

#include "BuildingGenerateTypes.h"

#include "BuildingHelperBase.generated.h"

class AViewerPawnBase;

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ABuildingHelperBase :
	public AActor
{
	GENERATED_BODY()

public:

	/**
	 * 默认机位
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<AViewerPawnBase> DefaultBuildingCameraSeat;
	
	/**
	 * 预设机位
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, TSoftObjectPtr<AViewerPawnBase>> PresetBuildingCameraSeat;
	
};
