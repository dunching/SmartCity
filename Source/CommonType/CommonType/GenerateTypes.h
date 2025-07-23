// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.generated.h"

class ADatasmithSceneActor;

UENUM()
enum class EOperatorType: uint8
{
	kLeftMouseButton,

	kRightMouseButton,

	kNone,
};

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FSceneActorMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<TSoftObjectPtr<ADatasmithSceneActor>> DataSmithSceneActorsSet;
};

USTRUCT(Blueprintable, BlueprintType)
struct COMMONTYPE_API FSceneActorConditional
{
	GENERATED_BODY()

	FSceneActorConditional();;

	FSceneActorConditional(
		const TSet<FGameplayTag>& InConditionalSet
		);

	bool bIsOnlyBulding = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTagContainer ConditionalSet;
};

struct COMMONTYPE_API TSceneActorConditionalKeyFuncs :
	BaseKeyFuncs<
		FSceneActorConditional, FSceneActorConditional,
		false
	>
{
private:
	typedef BaseKeyFuncs<
		FSceneActorConditional, FSceneActorConditional,
		false
	> Super;

public:
	typedef typename Super::ElementInitType ElementInitType;
	typedef typename Super::KeyInitType KeyInitType;

	static KeyInitType GetSetKey(
		ElementInitType Element
		);

	static bool Matches(
		KeyInitType A,
		KeyInitType B
		);

	static uint32 GetKeyHash(
		KeyInitType Key
		);
};

COMMONTYPE_API uint32 GetTypeHash(
	const FSceneActorConditional& SceneActorConditional
	);
