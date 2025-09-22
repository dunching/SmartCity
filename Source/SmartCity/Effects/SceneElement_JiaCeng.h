// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "SceneElement_JiaCeng.generated.h"

class ASceneActor;
class ACharacterBase;
class USceneActorInteractionComponent;
class UDatasmithAssetUserData;

struct FSceneElementConditional;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class USceneElement_JiaCengInterface : public UInterface
{
	GENERATED_BODY()
};

class SMARTCITY_API ISceneElement_JiaCengInterface
{
	GENERATED_BODY()

public:
	/**
	 * 确认是否不处于夹层的显示状态
	 * @param ConditionalSet 
	 * @return 
	 */
	virtual bool ProcessJiaCengLogic(
		const FSceneElementConditional& ConditionalSet
		) const;

	virtual void CheckIsJiaCeng(
		UDatasmithAssetUserData* AUDPtr
		);

protected:

private:
	bool bIsJiaCengg = false;
};
