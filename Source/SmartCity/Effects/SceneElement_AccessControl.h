// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_AccessControl.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;
class UActorSequenceComponent;

/**
 * 门禁
 */
UCLASS()
class SMARTCITY_API ASceneElement_AccessControl :
	public ASceneElementBase
{
	GENERATED_BODY()

public:

	ASceneElement_AccessControl(
		const FObjectInitializer& ObjectInitializer
		);

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UActorSequenceComponent* ActorSequenceComponent = nullptr;

};
