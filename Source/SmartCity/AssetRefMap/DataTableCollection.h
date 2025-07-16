// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"

#include "DataTableCollection.generated.h"

class UDataTable;
class UPAD_Talent_Property;

struct FToolProxy;
struct FWeaponProxy;
struct FSkillProxy;
struct FCoinProxy;
struct FBasicProxy;

struct FTableRowProxy_TagExtendInfo;
struct FTableRowProxy_CharacterGrowthAttribute;
struct FTableRow_Regions;
struct FTableRow_TalenSocket;
struct FTableRow_RewardsItems_DefeatEnemy;

/**
 * DataTables
 */
UCLASS(BlueprintType, Blueprintable)
class SMARTCITY_API UDataTableCollection : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDataTableCollection();

	virtual void PostCDOContruct() override;

	static UDataTableCollection* GetInstance();

};
