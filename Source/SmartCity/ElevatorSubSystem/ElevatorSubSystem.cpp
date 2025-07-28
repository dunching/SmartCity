#include "ElevatorSubSystem.h"

#include "AssetRefMap.h"
#include "Elevator.h"
#include "Async/Async.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "TemplateHelper.h"
#include "Tools.h"

UElevatorSubSystem* UElevatorSubSystem::GetInstance()
{
	return Cast<UElevatorSubSystem>(
									 USubsystemBlueprintLibrary::GetWorldSubsystem(
										  GetWorldImp(),
										  UElevatorSubSystem::StaticClass()
										 )
									);
}

void UElevatorSubSystem::ElevatorMoveToFloor(
	const FGameplayTag& ElevatorTag,
	int32 FloorIndex
	)
{
	if (UAssetRefMap::GetInstance()->ElevatorMap.Contains(ElevatorTag))
	{
		UAssetRefMap::GetInstance()->ElevatorMap[ElevatorTag]->ChangeTargetFloorIndex(FloorIndex);
	}
}
