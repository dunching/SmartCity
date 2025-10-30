// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"

#include "GameOptions.h"
#include "FloorHelperBase.h"

#include "FloorHelper_Description.generated.h"

class UWidgetComponent;
class UTextRenderComponent;
class UStaticMeshComponent;
class AFloorHelper;
class USceneComponent;
class UTextBlock;


UCLASS()
class SMARTCITY_API UFloorDescriptionHelper : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FloorIndexText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FloorDecriptionText = nullptr;

};

UCLASS()
class SMARTCITY_API AFloorHelper_Description :
	public AActor
{
	GENERATED_BODY()

public:
	AFloorHelper_Description(
		const FObjectInitializer& ObjectInitializer
		);

	virtual  void BeginPlay() override;
	
	virtual void Tick(
		float DeltaTime
		) override;

	void SetFloor(
		AFloorHelper* InFloorPtr
		);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ArmMeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BorderScenePtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BorderAddScenePtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> FloorDecriptionText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator RotSpeed = FRotator(90);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator OffsetRot = FRotator(0.000000, 90.000000, 180.000000);

	TObjectPtr<AFloorHelper> FloorPtr = nullptr;
};
