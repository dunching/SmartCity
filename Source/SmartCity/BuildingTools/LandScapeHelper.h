// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "FloorHelperBase.h"
#include "LandScapeBase.h"

#include "LandScapeHelper.generated.h"

class URectLightComponent;
class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawnBase;
class UActorSequenceComponent;
class AFloorHelper_Description;
class ALandmark;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ALandScapeHelper :
	public ALandScapeBase
{
	GENERATED_BODY()

public:
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TSoftObjectPtr<ALandmark>> LandmarkAry;
	
};
