#include "SpaceSubSystem.h"

#include "AssetRefMap.h"
#include "Elevator.h"
#include "Async/Async.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "TemplateHelper.h"
#include "Tools.h"

USpaceSubSystem* USpaceSubSystem::GetInstance()
{
	return Cast<USpaceSubSystem>(
									 USubsystemBlueprintLibrary::GetWorldSubsystem(
										  GetWorldImp(),
										  USpaceSubSystem::StaticClass()
										 )
									);
}

void USpaceSubSystem::ElevatorMoveToFloor(
	const FGameplayTag& ElevatorTag,
	int32 FloorIndex
	)
{
	if (UAssetRefMap::GetInstance()->ElevatorMap.Contains(ElevatorTag))
	{
		UAssetRefMap::GetInstance()->ElevatorMap[ElevatorTag]->ChangeTargetFloorIndex(FloorIndex);
	}
}
