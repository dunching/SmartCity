// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_JiaCeng.h"

#include "SceneElement_DeviceBase.generated.h"

class AFloorHelper;
class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;
class URouteMarker;
class UDatasmithAssetUserData;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_DeviceBase :
	public ASceneElementBase,
	public ISceneElement_JiaCengInterface
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void BeginInteraction() override;

	virtual void EndInteraction() override;

	virtual void InitialSceneElement() override;

	virtual TMap<FString,FString> GetStateDescription()const override;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;
	
	virtual TSharedPtr<FJsonValue> GetSceneElementData() const override;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere )
	FGameplayTag DeviceType ;

	UPROPERTY(BlueprintReadWrite,EditAnywhere )
	TObjectPtr<AFloorHelper> BelongFloor ;

	bool bIsOpened = false;
	
};
