// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "MainHUDLayout.generated.h"

class UCanvasPanel;
class UTextBlock;
class USizeBox;
class UOverlay;

UCLASS()
class SMARTCITY_API UFeatureItem : public UUserWidget
{
	GENERATED_BODY()

public:
};

UCLASS()
class SMARTCITY_API UFeatureWheel : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	void InitalFeaturesItem(
		const FString& FeatureName,
		const TArray<FString>& Features
		);

private:
	void UpdatePosition();

protected:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* FeatureCanvas = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox = nullptr;

	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere
	)
	TSubclassOf<UFeatureItem> FeatureItemClass;

	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere
	)
	float UpdateRate = 1.f / 60;

	FTimerHandle TimerHandle;
};

UCLASS()
class SMARTCITY_API UMainHUDLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	void InitalFeaturesItem(
		const FString& FeatureName,
		const TArray<FString>& Features
		);

	void RemoveFeatures();
	
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere
	)
	TSubclassOf<UFeatureWheel> FeatureWheelClass;

	UFeatureWheel* FeatureWheelPtr = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UOverlay* OverlapPtr = nullptr;

};
