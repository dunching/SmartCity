// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "BuildingGenerateTypes.h"
#include "SceneElementBase.h"

#include "SceneElement_LandScape.generated.h"

class USceneComponent;

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ASceneElement_LandScape :
	public ASceneElementBase
{
	GENERATED_BODY()

public:
	ASceneElement_LandScape(
		const FObjectInitializer& ObjectInitializer
		);
	
	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	/**
	 * 网格体
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

};
