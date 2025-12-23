// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_RadarMode.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawnBase;
class APersonMark;
class AStaticMeshActor;
class ASceneElement_RadarSweep;
class IWebSocket;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_RadarMode :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:
	ASceneElement_RadarMode(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void BeginPlay() override;

	virtual void Tick(
		float DeltaTime
		) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual FBox GetComponentsBoundingBox(
		bool bNonColliding = false,
		bool bIncludeFromChildActors = false
		) const override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void InitialSceneElement() override;

	virtual void UpdateReletiveTransform(
		const FTransform& NewRelativeTransform
		) override;

	void UpdatePositions(
		const TMap<FString, FVector>& Pts
		);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> NetState_StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UStaticMesh> SweepMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMaterialInstance> SweepMatInst;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ASceneElement_RadarSweep> SweepActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 Deepth = 120;

private:
	virtual void EntryFocusDevice(
		) override;

	virtual void EntryViewDevice(
		) override;

	virtual void EntryShowDevice(
		) override;

	virtual void EntryShoweviceEffect(
		) override;

	virtual void QuitAllState(
		) override;

	FTimerHandle UpdateMeshTimerHandle;

	FTimerHandle QueryTimerHandle;

	TMap<FString, APersonMark*> GeneratedMarkers;

private:

	void ClearMarks();
	
	FTimerHandle ClearTimerHandle;

	
	TSharedPtr<IWebSocket> Socket;

	void InitialSocket();
	
	UFUNCTION(BlueprintCallable)
	void Connect();

	UFUNCTION(BlueprintCallable)
	void SendText(const FString& Message);

	UFUNCTION(BlueprintCallable)
	void Close(int32 Code = 1000, const FString& Reason = TEXT("Normal"));

	void BindEvents();

	virtual void QueryDeviceInfoComplete(
		bool bSuccess,
		const FString& ResponStr
		) override;


	// 简单重连参数（可按需增强）
	FTimerHandle ReconnectTimer;
	
	int32 ReconnectTry = 0;
	
	FString LastUrl;

	void ScheduleReconnect();
};
