// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "Algorithm.generated.h"

UCLASS()
class UTILS_API UKismetAlgorithm : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static TPair<FTransform, float> GetCameraSeat(const TSet<AActor*>& Actors,
	                                              const FRotator& Rot,
	                                              float FOV
	);
};
