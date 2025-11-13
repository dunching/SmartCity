// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"

#include "BuildingGenerateTypes.h"
#include "BuildingHelperBase.h"

#include "FloorHelperBase.generated.h"

class ANavagationPaths;
class UBoxComponent;

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API AFloorHelperBase :
	public ABuildingHelperBase
{
	GENERATED_BODY()

public:

	AFloorHelperBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;;
	
	virtual void PostActorCreated() override;
	
	virtual void PostInitializeComponents() override;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		) override;
	
	virtual TPair<FTransform, float> GetCameraSeat(
			const FRotator& Rot,
			float FOV
			)const;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> SceneComponentPtr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UBoxComponent> BoxComponentPtr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UBoxComponent> InnerBoxComponentPtr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<ANavagationPaths> NavagationPaths = nullptr;
	
	/**
	 * 显示这些指定层时，此楼显示
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer GameplayTagContainer;
	
	/**
	 * 这个楼层是那一层
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag FloorTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 FloorIndex = 0;
	
	/**
	 * 用作辅助，如夹层的公共区域
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsSup = false;
	
	/**
	 * 该层的所有引用
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSceneElementMap AllReference;

};
