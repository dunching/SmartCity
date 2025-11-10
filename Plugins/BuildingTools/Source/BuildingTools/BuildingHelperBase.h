// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"

#include "BuildingGenerateTypes.h"
#include "SceneElementBase.h"

#include "BuildingHelperBase.generated.h"

class AViewerPawnBase;
class ASceneElementCategory;

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ABuildingHelperBase :
	public ASceneElementBase
{
	GENERATED_BODY()

public:

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	virtual  TMap<FString, TSoftObjectPtr<AViewerPawnBase>> GetPresetBuildingCameraSeat()const;
	
	/**
	 * 默认机位
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<AViewerPawnBase> DefaultBuildingCameraSeat;

	TMap<FGameplayTag, TObjectPtr<ASceneElementCategory>> SceneElementCategoryMap;
	
protected:
	
	/**
	 * 预设机位
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, TSoftObjectPtr<AViewerPawnBase>> PresetBuildingCameraSeat;
	
};
