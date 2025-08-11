// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "BuildingGenerateTypes.h"

#include "SceneElementBase.generated.h"

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ASceneElementBase :
	public AActor
{
	GENERATED_BODY()

public:
	ASceneElementBase(
		const FObjectInitializer& ObjectInitializer
		);

	void Replace(
		const TSoftObjectPtr<AActor>& ActorRef
		);

	virtual void ReplaceImp(
		AActor* ActorPtr
		);
	
	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef
		);

	virtual void BeginInteraction();

	virtual void EndInteraction();

	virtual TMap<FString,FString> GetStateDescription()const;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		);

	FString SceneElementName;

	FSceneElementConditional CurrentConditionalSet;
};
