// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"

#include "BuildingGenerateTypes.h"

#include "SceneElementBase.generated.h"

class USceneComponent;

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

	virtual void InitialSceneElement();

	void Replace(
		const TSoftObjectPtr<AActor>& ActorRef,
		const TPair<FName, FString>& InUserData,
		const TMap<FName, FString>& NewUserData
		);

	virtual void ReplaceImp(
		AActor* ActorPtr,
		const TPair<FName, FString>& InUserData
		);

	/**
	 * 合并所有
	 * @param ActorRef
	 * @param InUserData
	 * @param NewUserData 
	 */
	virtual void Merge(
		const TSoftObjectPtr<AActor>& ActorRef,
		const TPair<FName, FString>& InUserData,
		 const TMap<FName, FString>& NewUserData
		);

	virtual void BeginInteraction();

	virtual void EndInteraction();

	/**
	 * 退出所有状态
	 */
	virtual void QuitAllState(
		);

	virtual TMap<FString, FString> GetStateDescription() const;

	virtual void SwitchInteractionType(
		const FSceneElementConditional& ConditionalSet
		);

	virtual TSharedPtr<FJsonValue> GetSceneElementData() const;

	void RevertOnriginalMat();

	void UpdateExtensionParamMap(
		const TMap<FString, FString>& NewExtensionParamMap,
		bool bImmediatelyUpdate
		);

#if WITH_EDITORONLY_DATA
	FString SceneElementName;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString SceneElementID;

	TMap<FName, FString> UserData;
	
protected:
	void SetTranslucentImp(
		const TArray<UStaticMeshComponent*>& Components,
		int32 Value,
		const TSoftObjectPtr<UMaterialInstance>& MatRef
		);

	void CacheOriginalMat(
		const TArray<UStaticMeshComponent*>& Components
		);

	FSceneElementConditional CurrentConditionalSet;

	TMap<FString, FString> ExtensionParamMap;

	UPROPERTY()
	TMap<UStaticMeshComponent*, FMaterialsCache> OriginalMaterials;
};
