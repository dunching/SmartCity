#include "GameOptions.h"

UGameOptions::UGameOptions()
{
}

UGameOptions::UGameOptions(
	const FObjectInitializer& ObjectInitializer
	):Super(ObjectInitializer)
{
}

UGameOptions* UGameOptions::GetInstance()
{
	return GEngine ? CastChecked<UGameOptions>(GEngine->GetGameUserSettings()) : nullptr;
}
