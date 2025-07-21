#include "MainHUD.h"

struct FMainHUD : public TStructVariable<FMainHUD>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName FocusCharacterSocket = TEXT("FocusCharacterSocket");
};

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AMainHUD::ShowHUD()
{
	Super::ShowHUD();

	InitalHUD();
}

void AMainHUD::InitalHUD()
{
}
