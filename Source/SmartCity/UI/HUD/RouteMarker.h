// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoverWidgetBase.h"

#include "Blueprint/UserWidget.h"

#include "RouteMarker.generated.h"

class UCanvasPanel;
class UTextBlock;
class USizeBox;
class UOverlay;

UCLASS()
class SMARTCITY_API URouteMarker : public UHoverWidgetReBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual FVector GetHoverPosition() override;

	TWeakObjectPtr<AActor> TargetActor;

	FString TextStr;

protected:
	
	UPROPERTY(meta = (BindWidget))
	USizeBox* TextSizeBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text = nullptr;

	UPROPERTY(meta = (BindWidget))
	USizeBox* HeightSizeBox = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 TextSize = 100;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Height = 150;
	
};
