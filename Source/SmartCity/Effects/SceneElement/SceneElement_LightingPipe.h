// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_LightingPipe.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;

/**
 * 材质自发光
 */
UCLASS()
class SMARTCITY_API ASceneElement_LightingPipe :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:
	ASceneElement_LightingPipe(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef,
		const TPair<FName, FString>& InUserData
		, const TMap<FName, FString>& NewUserData
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	virtual void EntryFocusDevice(
		) override;

	virtual void QuitFocusDevice(
		) override;

	virtual void EntryViewDevice(
		) override;

	virtual void QuitViewDevice(
		) override;

	virtual void EntryShowDevice(
		) override;

	virtual void QuitShowDevice(
		) override;

	virtual void EntryShoweviceEffect(
		) override;

	virtual void QuitShowDeviceEffect(
		) override;

	virtual void QuitAllState(
		) override;

protected:
	void SwitchLight(
		int32 Intensity,
		int32 Temperature,
		const FLinearColor& LightColor
		);

	void SetEmissiveValue(
		int32 Value,
		int32 Temperature,
		const FLinearColor& LightColor
		);

	/**
	 * 固定的网格体
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<UStaticMeshComponent*> StaticMeshComponentsAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> EmissiveMaterialInstance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName EmissiveValue = TEXT("自发光强度");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName Color = TEXT("Color");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<URouteMarker> RouteMarkerPtr = nullptr;

	TPair<FName, FString> UserData;

	FString PWR_ID;
};
