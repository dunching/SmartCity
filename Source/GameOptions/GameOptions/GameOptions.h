// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "GameplayTagContainer.h"

#include "GameOptions.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct GAMEOPTIONS_API FGameplayFeatureKeyMap
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FKey Key;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString CMD;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Description;
};

UCLASS(BlueprintType, Blueprintable)
class GAMEOPTIONS_API UGameOptions : public UGameUserSettings
{
	GENERATED_BODY()

public:
	static UGameOptions* GetInstance();

#pragma region 按键映射
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey ClickItem = EKeys::LeftMouseButton;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey RotBtn = EKeys::LeftMouseButton;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey MoveBtn = EKeys::RightMouseButton;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey MouseX = EKeys::MouseX;
#pragma endregion

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSubclassOf<AActor>>NeedReplaceMap;
	
protected:
	
private:

};
