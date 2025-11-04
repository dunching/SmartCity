// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_RollerBlind.generated.h"

class UActorSequence;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_RollerBlind :
	public ASceneElement_DeviceBase
{
	GENERATED_BODY()

public:

	ASceneElement_RollerBlind(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

protected:

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
	
	/**
	 * 0 ~ 1
	 * 0: 完全关闭
	 * 1：完全打开
	 * @param Percent 
	 */
	void PlayAnimation(float Percent);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Duration = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UActorSequenceComponent> MySequenceComponent = nullptr;
	
};
