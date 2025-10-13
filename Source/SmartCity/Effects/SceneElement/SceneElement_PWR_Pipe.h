// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "GenerateTypes.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_PWR_Pipe.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UMaterialInterface;
class UStaticMeshComponent;

/**
 * 管道效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_PWR_Pipe :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:
	ASceneElement_PWR_Pipe(
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

	FString GetID()const;
	
	float EnergyValue = 0.f;

protected:

	virtual void EntryFocusDevice(
		) override;

	virtual void EntryViewDevice(
		) override;

	virtual void EntryShowevice(
		) override;

	virtual void EntryShoweviceEffect(
		) override;

	virtual void QuitAllState(
		) override;
	
	TPair<FName, FString> UserData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

};
