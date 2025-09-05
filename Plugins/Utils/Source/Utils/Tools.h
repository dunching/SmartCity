// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Tools.generated.h"

#define GENERATIONCLASSINFO(ThisClassType, ParentClassType) \
using ThisClass = ThisClassType; \
using Super = ParentClassType;

#define GENERATIONCLASSINFOONLYTHIS(ThisClassType) \
using ThisClass = ThisClassType; 

extern UTILS_API bool GIsExiting;

UTILS_API UWorld* GetWorldImp();

USTRUCT(BlueprintType)
struct FMaterialAry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<class UMaterialInterface*>MaterialsAry;
};
