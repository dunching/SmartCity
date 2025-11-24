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
	ABuilding_CurtainWall(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;
	
	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	void GenerateRollerBlind(AActor* ActorPtr);
	
protected:
	
	void OnHourChanged(int32 Hour);
	
	void OnExternalWall();

	void UpdateCollisionBox(const TArray<UStaticMeshComponent*>& SMCompsAry);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UStaticMeshComponent*>StaticMeshComponentsAry;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionComponentHelper = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<ULocalLightComponent>> RectLightComponentAry;
	
};
