// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "BuildingGenerateTypes.h"

#include "SceneElementBase.generated.h"

/**
 * 门禁
 */
UCLASS()
class BUILDINGTOOLS_API ASceneElementBase :
	public AActor
{
	GENERATED_BODY()

public:
	ASceneElementBase(
		const FObjectInitializer& ObjectInitializer
		);

	void Replace(
		const TSoftObjectPtr<AActor>& ActorRef,
		const TPair<FName, FString>& InUserData
		);

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		);

	/**
	 * 合并所有
	 * @param ActorRef
	 * @param InUserData 
	 */
	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef,
		const TPair<FName, FString>& InUserData
		);

	virtual void BeginInteraction();

	virtual void EndInteraction();

	virtual TMap<FString, FString> GetStateDescription() const;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		);
	
	virtual TSharedPtr<FJsonValue> GetSceneElementData() const;
	
	void RecordOnriginalMat();

	void RevertOnriginalMat();
	
#if WITH_EDITORONLY_DATA
	FString SceneElementName;
#endif

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString DeviceID;

	FSceneElementConditional CurrentConditionalSet;
	
	UPROPERTY()
	TMap<UStaticMeshComponent*, FMaterialsCache>OriginalMaterials;

};
