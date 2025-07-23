// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "AssetRefBase.h"
#include "GenerateTypes.h"

#include "AssetRefMap.generated.h"

class AActor;
class UDataLayerAsset;
class ADatasmithSceneActor;

UCLASS(BlueprintType, Blueprintable)
class SMARTCITY_API UAssetRefMap : public UAssetRefBase
{
	GENERATED_BODY()
public:

	UAssetRefMap();

	static UAssetRefMap* GetInstance();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSubclassOf<AActor>>NeedReplaceMap;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FSceneActorConditional, FSceneActorMap> SceneActorMap;
	
	/**
	 * 设备ID的前缀
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Datasmith_UniqueId;
	
	/**
	 * 设备类型的Key
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FString>CatogoryPrifix;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString FJPG;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString XFJZ;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Space;
	
};
