// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "SceneElementBase.generated.h"

enum class EInteractionType : uint8
{
	kRegular,		
	kView,
	kFocus,
	kHide,
	kNone,
};

/**
 * 门禁
 */
UCLASS()
class COMMONTYPE_API ASceneElementBase :
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
		EInteractionType InInteractionType
		);

	FString SceneElementName;

	EInteractionType CurrentInteractionType = EInteractionType::kNone;
};
