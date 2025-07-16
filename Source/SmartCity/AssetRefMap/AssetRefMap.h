// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "AssetRefBase.h"

#include "AssetRefMap.generated.h"

class USoundMix;
class USoundClass;
class UTexture2D;
class UStaticMesh;
class UMaterialInstance;
class USkeletalMesh;
class UGameplayEffect;

class UMainUILayout;
class AGroupManagger;
class URaffleMenu;
class UMainHUDLayout;
class UPawnStateActionHUD;
class UPawnStateBuildingHUD;
class UItemProxyDragDropOperation;
class UItemProxyDragDropOperationWidget;
class UAllocationableProxyDragDropOperationWidget;
class UAllocationToolsMenu;
class UAllocationSkillsMenu;
class UBackpackMenu;
class UCharacterTitle;
class UEffectsList;
class UProgressTips;
class UTalentAllocation;
class UGroupManaggerMenu;
class UHUD_TeamInfo;
class URegionPromt;
class UUpgradePromt;
class UFocusIcon;
class UMarkPoints;
class UUpgradeBoder;
class UCharacterRisingTips;
class UInteractionList;
class AHumanCharacter;
class AHorseCharacter;
class AGeneratorNPC;
class UMainMenuLayout;
class UGE_Damage;
class UGE_Duration;
class UGE_Damage_Callback;
class AMinimapSceneCapture2D;
class ATargetPoint_Runtime;
class AQuestChain_Branch;
class AGuideThread_Challenge;

UCLASS(BlueprintType, Blueprintable)
class SMARTCITY_API UAssetRefMap : public UAssetRefBase
{
	GENERATED_BODY()
public:

	UAssetRefMap();

	static UAssetRefMap* GetInstance();

};
