// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"

#include "MessageBody.generated.h"

class FDecoratorBase;

USTRUCT()
struct FMessageBody
{
	GENERATED_BODY()
	
	FMessageBody();

	virtual ~FMessageBody();

	virtual TSharedPtr<FJsonObject> Deserialize(const FString& JsonStr);

	FString GetJsonString()const;

	FString GetName()const;
	
public:
	
	virtual TSharedPtr<FJsonObject> SerializeBody()const;

	FString Name;

	FGuid Guid;
};

USTRUCT()
struct FMessageBody_SelectedSpace : public FMessageBody
{
	
	GENERATED_BODY()
	
	FMessageBody_SelectedSpace();
	
	virtual TSharedPtr<FJsonObject> Deserialize(const FString& JsonStr) override;

	FString SpaceName;

protected:
	
	virtual TSharedPtr<FJsonObject> SerializeBody()const override;

};

USTRUCT()
struct FMessageBody_SelectedDevice : public FMessageBody
{
	
	GENERATED_BODY()
	
	FMessageBody_SelectedDevice();
	
	FString DeviceID;

protected:
	
	virtual TSharedPtr<FJsonObject> SerializeBody()const override;

};

