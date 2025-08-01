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

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Radar =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Radar")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_QD =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.QD")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_RD =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.RD")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_AccessControl =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.AccessControl")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Lighting =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Lighting")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_HVAC =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.HVAC")));

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