// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateTypes.h>

#include "GameplayTagContainer.h"

#include "DataTableCollection.generated.h"

class UDataTable;
class UPAD_Talent_Property;

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
