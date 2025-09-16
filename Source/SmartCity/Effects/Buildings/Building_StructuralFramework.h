// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "GameOptions.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"
#include "Tools.h"

#include "Building_StructuralFramework.generated.h"

class UStaticMeshComponent;

/**
 * 墙
 */
UCLASS()
class SMARTCITY_API ABuilding_StructuralFramework :
	public ASceneElementBase
{
	GENERATED_BODY()

public:
	ABuilding_StructuralFramework(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void ReplaceImp(
		AActor* ActorPtr
		) override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;

	enum class EState : uint8
	{
		kOriginal,
		kTranslucent,
		kHiden,
	};
	
	void SwitchState(EState State);
		
	/**
	 * 网格体
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TMap<UStaticMeshComponent*, FMaterialAry> MaterialMap;

	FGameplayTag Floor;
};
