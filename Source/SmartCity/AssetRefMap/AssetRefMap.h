// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "AssetRefBase.h"

#include "AssetRefMap.generated.h"

class AActor;
class UDataLayerAsset;

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
	TMap<FGameplayTag, TSoftObjectPtr<UDataLayerAsset>>DataLayerAssetMap;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<TSoftObjectPtr<UDataLayerAsset>>AllDataLayerAssetMap;
	
};
