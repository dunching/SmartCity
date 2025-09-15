// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GenerateTypes.generated.h"

class ADatasmithSceneActor;
class AReplaceActor;
class ASceneElementBase;

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FMaterialsCache
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<UMaterialInterface*> MaterialsCacheAry;
};

UENUM()
enum class EOperatorType: uint8
{
	kLeftMouseButton,

	kRightMouseButton,

	kNone,
};

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FModeDecription
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Title;
};

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FSceneElementReplaceHelper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bOnlyKey = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Key;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Value;

};

struct COMMONTYPE_API TFSceneElementTypeHelperKeyFuncs :
	BaseKeyFuncs<
		FSceneElementReplaceHelper, FSceneElementReplaceHelper,
		false
	>
{
private:
	typedef BaseKeyFuncs<
		FSceneElementReplaceHelper, FSceneElementReplaceHelper,
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
	const FSceneElementReplaceHelper& SceneElementTypeHelper
	);

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FSceneElementMergeHelper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bOnlyKey = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Key;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Value;

};

COMMONTYPE_API uint32 GetTypeHash(
	const FSceneElementMergeHelper& SceneElementTypeHelper
	);

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FSceneSpace
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Key;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Value;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString SpaceNameValue;

};

COMMONTYPE_API uint32 GetTypeHash(
	const FSceneSpace& SceneElementTypeHelper
	);

