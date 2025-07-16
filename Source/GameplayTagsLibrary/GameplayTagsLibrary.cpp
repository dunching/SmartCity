#include "GameplayTagsLibrary.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

FGameplayTag UGameplayTagsLibrary::BaseFeature_Reply =
	FGameplayTag::RequestGameplayTag(FName(TEXT("BaseFeature.Reply")));
