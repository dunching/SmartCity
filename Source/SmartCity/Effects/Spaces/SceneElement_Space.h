// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_Space.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class URouteMarker;
class ASceneElement_DeviceBase;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_Space :
	public ASceneElementBase
{
	GENERATED_BODY()

public:
	ASceneElement_Space(
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
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	virtual TSharedPtr<FJsonValue> GetSceneElementData() const override;
	
	void SetFeatures(const TArray<FString>& InFeaturesAry);

	TSet<ASceneElement_DeviceBase*>GetAllDevices()const;
	
	FString Category;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString DataSmith_Key = TEXT("Element*空间划分");
protected:

	void SwitchColor(const FColor& Color);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<URouteMarker> RouteMarkerPtr = nullptr;
	
	TArray<FString> FeaturesAry;
};
