// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "FloorHelperBase.h"

#include "FloorHelper.generated.h"

class URectLightComponent;
class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawnBase;
class UActorSequenceComponent;
class AFloorHelper_Description;

UCLASS(BlueprintType, Blueprintable)
class SMARTCITY_API UControlBorderDevices : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<AActor> P2;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSoftObjectPtr<AActor>>Devices;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSoftObjectPtr<AActor>>CurtainWall;
};

/**
 * 楼层
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

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void BeginPlay() override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	void OnHourChanged(
		int32 Hour
		);

	void OnExternalWall();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> FloorHelper_DescriptionAttachTransform;

	TObjectPtr<AFloorHelper_Description> FloorHelper_DescriptionPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString FloorDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString FloorIndexDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AViewerPawnBase> ViewerPawnClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<TSoftObjectPtr<UControlBorderDevices>> ControlBorderMaps;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<URectLightComponent>> RectLightComponentAry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDisplayDescritpion = true;
};

UCLASS()
class SMARTCITY_API AFloorHelper_Computer :
	public AFloorHelper
{
	GENERATED_BODY()

public:
	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual TMap<FString, TSoftObjectPtr<AViewerPawnBase>> GetPresetBuildingCameraSeat() const override;
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

	void SetFloor(
		AFloorHelper* FloorPtr
		);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
};
