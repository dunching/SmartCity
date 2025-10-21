// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "FloorHelperBase.h"

#include "FloorHelper_Description.generated.h"

class UTextRenderComponent;
class UStaticMeshComponent;
class AFloorHelper;
class USceneComponent;

UCLASS()
class SMARTCITY_API AFloorHelper_Description :
	public AActor
{
	GENERATED_BODY()

public:
	AFloorHelper_Description(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void Tick(float DeltaTime) override;
	
	void SetFloor(AFloorHelper*FloorPtr);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ArmMeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BorderScenePtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BorderAddScenePtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeftBorderMeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MidBorderMeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> RightBorderMeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextRenderComponent> FloorIndexText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextRenderComponent> FloorDecriptionText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator RotSpeed = FRotator(90);

};
