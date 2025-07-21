#include "UIManagerSubSystem.h"

#include "Async/Async.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "TemplateHelper.h"
#include "Tools.h"

struct FUIManagerSubSystem : public TStructVariable<FUIManagerSubSystem>
{
	FName GetItemInfos_Socket = TEXT("GetItemInfos_Socket");

	FName RaffleMenu_Socket = TEXT("RaffleMenu_Socket");

	FName AllocationSkills_Socket = TEXT("AllocationSkills_Socket");

	FName PawnStateConsumablesHUD_Socket = TEXT("PawnStateConsumablesHUD_Socket");

	FName PawnActionStateHUDSocket = TEXT("PawnActionStateHUDSocket");

	FName FocusCharacterSocket = TEXT("FocusCharacterSocket");
};

UUIManagerSubSystem* UUIManagerSubSystem::GetInstance()
{
	return Cast<UUIManagerSubSystem>(
	                                 USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		                                  GetWorldImp(),
		                                  UUIManagerSubSystem::StaticClass()
		                                 )
	                                );
}

UUIManagerSubSystem::UUIManagerSubSystem() :
                                           Super()
{
}

UUIManagerSubSystem::~UUIManagerSubSystem()
{
}

void UUIManagerSubSystem::Initialize(
	FSubsystemCollectionBase& Collection
	)
{
	Super::Initialize(Collection);
}
