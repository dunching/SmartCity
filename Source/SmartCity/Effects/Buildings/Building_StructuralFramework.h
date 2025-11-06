// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building_ItemBase.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"
#include "Tools.h"

#include "Building_StructuralFramework.generated.h"

class UStaticMeshComponent;

/**
 * 建筑类的
 */
UCLASS()
class SMARTCITY_API ABuilding_StructuralFramework :
	public ABuilding_ItemBase
{
	GENERATED_BODY()

public:
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

};