// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"

#include "SceneElement_RadarSweep.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawnBase;
class APersonMark;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_RadarSweep :
	public AActor
{
	GENERATED_BODY()

public:
	ASceneElement_RadarSweep(
		const FObjectInitializer& ObjectInitializer
		);

	UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent = nullptr;

};
