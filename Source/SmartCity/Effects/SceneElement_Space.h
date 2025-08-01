// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_Space.generated.h"

class UPlayerComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class AViewerPawn;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_Space :
	public ASceneElementBase
{
	GENERATED_BODY()

public:
	ASceneElement_Space(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual void ReplaceImp(
		AActor* ActorPtr
		) override;

	virtual void SwitchFocusState(bool bIsFocus) override;

	FString Category;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString DataSmith_Key = TEXT("Element*空间划分");
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;
};
