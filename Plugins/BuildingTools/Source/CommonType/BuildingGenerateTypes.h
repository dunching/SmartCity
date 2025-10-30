// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "BuildingGenerateTypes.generated.h"

class AReplaceActorBase;
class ADatasmithSceneActor;
class ASceneElementBase;

USTRUCT(BlueprintType, Blueprintable)
struct BUILDINGTOOLS_API FMaterialsCache
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<UMaterialInterface*> MaterialsCacheAry;
};

USTRUCT(BlueprintType, Blueprintable)
struct BUILDINGTOOLS_API FSceneElementFilter
{
	GENERATED_BODY()

	/**
	 * Datasmith的挂载
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSoftObjectPtr<ADatasmithSceneActor>> DatasmithSceneActorSet;

	/**
	 * 自定义的挂载
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSoftObjectPtr<ASceneElementBase>> OtherItem;
};

USTRUCT(BlueprintType, Blueprintable)
struct BUILDINGTOOLS_API FSceneElementMap
{
	GENERATED_BODY()

	/**
	 * 硬装，结构
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSceneElementFilter StructItemSet;

	/**
	 * 硬装，内饰
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSceneElementFilter InnerStructItemSet;

	/**
	 * 软装，如电气
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSceneElementFilter SoftDecorationItem;

	/**
	 * 空间区域
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSceneElementFilter SpaceItemSet;
};

/**
 * 条件
 */
USTRUCT(Blueprintable, BlueprintType)
struct BUILDINGTOOLS_API FSceneElementConditional
{
	GENERATED_BODY()

	FSceneElementConditional();;

	FSceneElementConditional(
		const TSet<FGameplayTag>& InConditionalSet
		);


	// 重载 == 操作符
	bool operator==(
		const FSceneElementConditional& Other
		) const
	{
		return ConditionalSet == Other.ConditionalSet;
	}

	// 声明 GetTypeHash 为 friend 函数
	friend uint32 GetTypeHash(
		const FSceneElementConditional& CustomStruct
		)
	{
		uint32 HashCode = 0;
		for (const auto& Iter : CustomStruct.ConditionalSet)
		{
			HashCode = HashCombine(GetTypeHash(Iter), HashCode);
		}
		return HashCode;
	}

	static FSceneElementConditional EmptyConditional;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer ConditionalSet;
};
