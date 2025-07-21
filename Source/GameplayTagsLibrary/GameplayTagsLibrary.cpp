#include "GameplayTagsLibrary.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

FGameplayTag UGameplayTagsLibrary::Seat_Default =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Seat.Default")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode")));

FGameplayTag UGameplayTagsLibrary::Interaction_Mode_Scene =
	FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Mode.Scene")));

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
