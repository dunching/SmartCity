#include "OpenWorldSystem.h"

#include "AssetRefMap.h"
#include "LogWriter.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Tools.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

UOpenWorldSubSystem* UOpenWorldSubSystem::GetInstance()
{
	return Cast<UOpenWorldSubSystem>(
	                                 USubsystemBlueprintLibrary::GetWorldSubsystem(
		                                  GetWorldImp(),
		                                  UOpenWorldSubSystem::StaticClass()
		                                 )
	                                );
}
