// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building_ItemBase.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"
#include "Tools.h"

#include "Building_Pillar.generated.h"

class UStaticMeshComponent;

/**
 * 墙
 */
UCLASS()
class SMARTCITY_API ABuilding_Pillar :
	public ABuilding_ItemBase
{
	GENERATED_BODY()

public:
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

};
