// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_Lighting.generated.h"

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
class SMARTCITY_API ASceneElement_Lighting :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:
	
	ASceneElement_Lighting(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;
	
	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

protected:

	void SwitchLight(int32 Intensity);
	
	void SetEmissiveValue(int32 Value);
	
	/**
	 * 固定的网格体
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> EmissiveMaterialInstance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName EmissiveValue = TEXT("Emissive");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<URouteMarker> RouteMarkerPtr = nullptr;

	TPair<FName, FString> UserData;

	FString PWR_ID ;
	
};
