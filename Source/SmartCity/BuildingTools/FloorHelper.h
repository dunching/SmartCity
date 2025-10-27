// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "FloorHelperBase.h"

#include "FloorHelper.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;
class AFloorHelper_Description;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API AFloorHelper :
	public AFloorHelperBase
{
	GENERATED_BODY()

public:

	AFloorHelper(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> FloorHelper_DescriptionAttachTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString FloorDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString FloorIndexDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDisplayDescritpion = true;

	TObjectPtr<AFloorHelper_Description>FloorHelper_DescriptionPtr = nullptr;
};

UCLASS()
class SMARTCITY_API ABuilding_Floor_Mask :
	public AActor
{
	GENERATED_BODY()

public:
	ABuilding_Floor_Mask(
		const FObjectInitializer& ObjectInitializer
		);

	void SetFloor(AFloorHelper*FloorPtr);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

};
