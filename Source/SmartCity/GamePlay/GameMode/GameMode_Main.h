// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"

#include "GameMode_Main.generated.h"

extern SMARTCITY_API class AShiYuGameMode* GGameModePtr;

class UInventoryComponent;
struct FCharacterProxy;

/**
 *
 */
UCLASS()
class SMARTCITY_API AGameMode_Main : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGameMode_Main();
	
protected:

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay()override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void BeginDestroy()override;

private:

	void ProcessExternalWall();
	
	void ProcessFloor();
	
	void ProcessSpace();
	
};
