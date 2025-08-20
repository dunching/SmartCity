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
struct COMMONTYPE_API FSceneElementTypeHelper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Key;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Value;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bNeedMerge = false;
};

struct COMMONTYPE_API TFSceneElementTypeHelperKeyFuncs :
	BaseKeyFuncs<
		FSceneElementTypeHelper, FSceneElementTypeHelper,
		false
	>
{
private:
	typedef BaseKeyFuncs<
		FSceneElementTypeHelper, FSceneElementTypeHelper,
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
	const FSceneElementTypeHelper& SceneElementTypeHelper
	);
