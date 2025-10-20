// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "MainHUDLayout.generated.h"

class UCanvasPanel;
class UTextBlock;
class USizeBox;
class UOverlay;
class UButton;
class ASceneElementBase;
class UFeatureWheel;
class ASceneElement_Space;

struct FFeaturesItem;

UCLASS()
class SMARTCITY_API UMainHUDLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void InitalFeaturesItem(
		ASceneElement_Space * SceneElement_SpacePtr,
		const FString& FeatureName,
		const TArray<FFeaturesItem>& Features
		);

	void RemoveFeatures();
	
	ASceneElement_Space * SceneElement_SpacePtr = nullptr;
	
	UFeatureWheel* FeatureWheelPtr = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UOverlay* OverlapPtr = nullptr;

};
