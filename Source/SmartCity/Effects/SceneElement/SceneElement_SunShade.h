// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_SunShade.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;
class URouteMarker;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_SunShade :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:

	ASceneElement_SunShade(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	void UpdateAngle(int32 Angle);

protected:
	void UpdateAngleImp();

	void RemoveRouteMarker();
	
	/**
	 * 固定的网格体
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ChestMeshComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> FanAncherMeshComponent = nullptr;
	
	/**
	 * 旋转的网格体
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> FanMeshComponent = nullptr;

	int32 TargetAngle = 0;
	
	int32 CurrentAngle = 0;
	
	/**
	 * 开合速度
	 */
	int32 Speed = 30;

	float Frequency = 1.f / 24;
	
	FTimerHandle TimerHandle;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<URouteMarker> RouteMarkerClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<URouteMarker> RouteMarkerPtr = nullptr;

};
