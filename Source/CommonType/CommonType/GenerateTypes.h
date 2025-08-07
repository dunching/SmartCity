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

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FSceneElementFilter
{
	GENERATED_BODY()

	/**
	 * 要显示的
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<TSoftObjectPtr<ADatasmithSceneActor>> DatasmithSceneActorSet;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<TSoftObjectPtr<AReplaceActor>> ReplaceActorSet;

	/**
	 * 通过用户数据过滤
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FString, FString> DataSmithUserAssetDataMap;

	/**
	 * 通过类型过滤
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<TSubclassOf<ASceneElementBase>> TypeSet;
};

USTRUCT(BlueprintType, Blueprintable)
struct COMMONTYPE_API FSceneElementMap
{
	GENERATED_BODY()

	/**
	 * 硬装，结构
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FSceneElementFilter StructItemSet;

	/**
	 * 硬装，内饰
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FSceneElementFilter InnerStructItemSet;

	/**
	 * 软装，如电气
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FSceneElementFilter SoftDecorationItem;

	/**
	 * 空间区域
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FSceneElementFilter SpaceItemSet;
};

/**
 * 条件
 */
USTRUCT(Blueprintable, BlueprintType)
struct COMMONTYPE_API FSceneElementConditional
{
	GENERATED_BODY()

	FSceneElementConditional();;

	FSceneElementConditional(
		const TSet<FGameplayTag>& InConditionalSet
		);


	// 重载 == 操作符
	bool operator==(const FSceneElementConditional& Other) const
	{
		return ConditionalSet == Other.ConditionalSet;
	}
	// 声明 GetTypeHash 为 friend 函数
	friend uint32 GetTypeHash(const FSceneElementConditional& CustomStruct)
	{
		uint32 HashCode = 0;
		for (const auto& Iter : CustomStruct.ConditionalSet)
		{
			HashCode = HashCombine(GetTypeHash(Iter), HashCode);
		}
		return HashCode;
	}
	
	static FSceneElementConditional EmptyConditional;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTagContainer ConditionalSet;
};
