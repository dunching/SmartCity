// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

#include "Landmark.generated.h"

class UNiagaraComponent;
class USceneComponent;
class UWidgetComponent;
class UTextBlock;


UCLASS()
class SMARTCITY_API ULandmarkHelper : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FloorIndexText = nullptr;
};

/*
 * 用于表示人物的标记点
 */
UCLASS()
class SMARTCITY_API ALandmark : public AActor
{
	GENERATED_BODY()

public:
	ALandmark(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void BeginPlay() override;

	void SetText(
		const FString& InText
		);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString LandmarkName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ArmMeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BorderScenePtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BorderAddScenePtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> FloorDecriptionText = nullptr;
};
