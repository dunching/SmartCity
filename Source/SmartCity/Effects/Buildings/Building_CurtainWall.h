// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building_ItemBase.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"
#include "Tools.h"

#include "Building_CurtainWall.generated.h"

class UStaticMeshComponent;
class ULocalLightComponent;

/**
 * 幕墙
 */
UCLASS()
class SMARTCITY_API ABuilding_CurtainWall :
	public ABuilding_ItemBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	enum class EState : uint8
	{
		kOriginal,
		kHiden,
	};
	
	void SwitchState(EState State);

protected:
	
	void OnHourChanged(int32 Hour);
	
	void OnExternalWall();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ULocalLightComponent>> RectLightComponentAry;
	
};
