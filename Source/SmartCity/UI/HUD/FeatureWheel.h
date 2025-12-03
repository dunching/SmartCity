// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "HoverWidgetBase.h"

#include "FeatureWheel.generated.h"

class UVerticalBox;
class UCanvasPanel;
class UTextBlock;
class USizeBox;
class UOverlay;
class UButton;
class ASceneElementBase;
class UFeatureWheel;

USTRUCT(BlueprintType, Blueprintable)
struct SMARTCITY_API FFeaturesItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Text;

	TObjectPtr<ASceneElementBase>SceneElementPtr = nullptr;
};

UCLASS()
class SMARTCITY_API UFeatureItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	void InitalFeaturesItem(
		const FFeaturesItem& InFeatureItem
		);

protected:

	void UpdateStateDescription();
	
	UFUNCTION()
	void OnClicked();
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Button = nullptr;

	FFeaturesItem FeaturesItem;
};

UCLASS()
class SMARTCITY_API UFeatureWheel : public UHoverWidgetReBase
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	virtual FVector GetHoverPosition() override;

	void InitalFeaturesItem(
		const FString& FeatureName,
		const TArray<FFeaturesItem>& Features
		);

	void SetOncliced(const FOnButtonClickedEvent&OnClicked);

	// void SetViewType(bool bIs);
	
	FVector TargetPt = FVector::ZeroVector;

private:

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Button = nullptr;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VerticalBoxPtr = nullptr;

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
