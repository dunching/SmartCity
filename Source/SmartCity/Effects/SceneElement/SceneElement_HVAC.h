// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_HVAC.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;
class UNiagaraComponent;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_HVAC :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:
	ASceneElement_HVAC(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void EntryFocusDevice(
		) override;

	virtual void QuitFocusDevice(
		) override;

	virtual void EntryViewDevice(
		) override;

	virtual void QuitViewDevice(
		) override;

	virtual void EntryShowevice(
		) override;

	virtual void QuitShowDevice(
		) override;
	
	virtual void EntryShoweviceEffect(
		) override;

	virtual void QuitShowDeviceEffect(
		) override;
	
	virtual void QuitAllState(
		) override;
	
	/**
	 * 网格体
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> NiagaraComponentPtr = nullptr;
};
