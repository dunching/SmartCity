// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SEJC.h"

#include "SceneElement_Regualar.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;
class URouteMarker;
class UDatasmithAssetUserData;
class AFloorHelper;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_Regualar :
	public ASceneElementBase,
	public ISceneElement_JiaCengInterface
{
	GENERATED_BODY()

public:
	ASceneElement_Regualar(
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

	virtual void BeginInteraction() override;

	virtual void EndInteraction() override;

	virtual TMap<FString, FString> GetStateDescription() const override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

protected:
	virtual void QuitAllState(
		) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag DeviceType;

	bool bIsOpened = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<AFloorHelper> BelongFloor = nullptr;

};
