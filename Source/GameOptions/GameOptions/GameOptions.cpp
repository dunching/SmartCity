#include "GameOptions.h"

UGameOptions* UGameOptions::GetInstance()
{
	return GEngine ? CastChecked<UGameOptions>(GEngine->GetGameUserSettings()) : nullptr;
}
