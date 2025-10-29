// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"

#include "BuildingGenerateTypes.h"

#include "LandScapeBase.generated.h"

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ALandScapeBase :
	public ABuildingHelperBase
{
	GENERATED_BODY()

public:

	/**
	 * 该楼宇的所有引用
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSceneElementMap AllReference;

};
