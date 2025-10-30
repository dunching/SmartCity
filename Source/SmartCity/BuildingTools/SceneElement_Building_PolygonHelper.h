// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"

#include "SceneElement_Building_PolygonHelper.generated.h"

class USplineComponent;
class USplineMeshComponent;

/**
 * 雷达扫描效果
 */
UCLASS()
class SMARTCITY_API ASceneElement_Building_PolygonHelper :
	public ASceneElementBase
{
	GENERATED_BODY()

public:
	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

protected:
	
	virtual void EntryFocusDevice(
		const FSceneElementConditional& ConditionalSet
		) ;

	virtual void EntryShowevice(
		const FSceneElementConditional& ConditionalSet
		) ;

	virtual void EntryShowEffect(
		const FSceneElementConditional& ConditionalSet
		) ;

	virtual void QuitAllState(
		) override;
	
};
