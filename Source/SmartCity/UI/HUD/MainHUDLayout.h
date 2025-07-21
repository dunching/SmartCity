// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "MainHUDLayout.generated.h"

class UCanvasPanel;
class UTextBlock;
class USizeBox;

UCLASS()
class SMARTCITY_API UFeatureItem : public UUserWidget
{
	GENERATED_BODY()

public:
};

UCLASS()
class SMARTCITY_API UMainHUDLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitalFeaturesItem(
		const FString& FeatureName,
		const TArray<FString>& Features
		);

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
};
