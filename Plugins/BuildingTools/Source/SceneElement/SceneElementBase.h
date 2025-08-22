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
	
	/**
	 * 合并所有
	 * @param ActorRef 
	 */
	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef
		);

	/**
	 * 与附近的合并
	 * @param ActorRef 
	 */
	virtual void MergeWithNear(
		const TSoftObjectPtr<AActor>& ActorRef
		);

	virtual void BeginInteraction();

	virtual void EndInteraction();

	virtual TMap<FString,FString> GetStateDescription()const;
	
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		);

#if WITH_EDITORONLY_DATA
	FString SceneElementName;
#endif

	FSceneElementConditional CurrentConditionalSet;
};
