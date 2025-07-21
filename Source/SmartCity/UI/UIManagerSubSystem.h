// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GenerateTypes.h"

#include "UIManagerSubSystem.generated.h"

struct FPawnDataStruct;
struct FSceneTool;


class UMainHUDLayout;
class UMainMenuLayout;

/*
	HUD、UI
*/
UCLASS()
class SMARTCITY_API UUIManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	template <typename FWidgetType>
	using FBeforeDisplayFunc = std::function<void(
		FWidgetType*
		)>;

	static UUIManagerSubSystem* GetInstance();

	UUIManagerSubSystem();

	virtual ~UUIManagerSubSystem();

	virtual void Initialize(
		FSubsystemCollectionBase& Collection
		) override;

protected:

private:
};
