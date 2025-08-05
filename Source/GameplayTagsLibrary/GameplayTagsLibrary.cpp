#include "GameplayTagsLibrary.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

FGameplayTag UGameplayTagsLibrary::Seat_Default =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Seat.Default")));


FGameplayTag UGameplayTagsLibrary::Interaction_Area =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Area")));

FGameplayTag UGameplayTagsLibrary::Interaction_Area_ExternalWall =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Area.ExternalWall")));

FGameplayTag UGameplayTagsLibrary::Interaction_Area_Floor =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Area.Floor")));

FGameplayTag UGameplayTagsLibrary::Interaction_Area_Floor_F1 =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Area.Floor.F1")));

FGameplayTag UGameplayTagsLibrary::Interaction_Area_Space =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Area.Space")));

FGameplayTag UGameplayTagsLibrary::Interaction_Area_SplitFloor =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Area.SplitFloor")));


FGameplayTag UGameplayTagsLibrary::Interaction_Mode =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Empty =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Empty")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Tour =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Tour")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Scene =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Scene")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_PWR =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.PWR")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_PWR_Lighting =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.PWR.Lighting")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_PWR_HVAC =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.PWR.HVAC")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_ELV =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.ELV")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_ELV_Radar =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.ELV.Radar")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_ELV_AccessControl =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.ELV.AccessControl")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_SunShade =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.SunShade")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Monitor =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Monitor")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Elevator =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Elevator")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Intelligence =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Intelligence")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Energy =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Energy")));


FGameplayTag UGameplayTagsLibrary::SceneElement_FanCoil =
	FGameplayTag::RequestGameplayTag(FName(TEXT("SceneElement.FanCoil")));