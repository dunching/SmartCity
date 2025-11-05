// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_Space.generated.h"

class UBoxComponent;
class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawnBase;
class URouteMarker;
class ASceneElement_DeviceBase;
class UFeatureWheel;
class AFloorHelper;
class ULocalLightComponent;

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

	virtual FBox GetComponentsBoundingBox(
		bool bNonColliding = false,
		bool bIncludeFromChildActors = false
		) const override;

	virtual void InitialSceneElement() override;

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

	virtual TSharedPtr<FJsonValue> GetSceneElementData() const override;
	
	void SetFeatures(const TArray<FString>& InFeaturesAry);

	TSet<ASceneElement_DeviceBase*>GetAllDevices()const;
	
	FString Category;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString DataSmith_Key = TEXT("Element*空间划分");
protected:

	virtual void EntryViewDevice(
		const FSceneElementConditional& ConditionalSet
		);

	virtual void EntryFocusDevice(
		const FSceneElementConditional& ConditionalSet
		) ;

	virtual void EntryShow(
		const FSceneElementConditional& ConditionalSet
		) ;

	virtual void EntryShowEffect(
		const FSceneElementConditional& ConditionalSet
		) ;

	virtual void QuitAllState(
		) override;
	
	void SwitchColor(const FColor& Color);

	UFUNCTION()
	void OnClickedTag();
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<UBoxComponent*>CollisionComponentsAry;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TObjectPtr<UFeatureWheel>> FeatureWheelAry;
	
	TArray<FString> FeaturesAry;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<AFloorHelper> BelongFloor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ULocalLightComponent>> RectLightComponentAry;
	
};
