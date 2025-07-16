
#include "DataTableCollection.h"

#include "GameplayTagsManager.h"
#include "Tools.h"
#include "PlanetWorldSettings.h"

UDataTableCollection::UDataTableCollection() :
	Super()
{
}

void UDataTableCollection::PostCDOContruct()
{
	Super::PostCDOContruct();
}

UDataTableCollection* UDataTableCollection::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetSceneProxyExtendInfoMap();
}
