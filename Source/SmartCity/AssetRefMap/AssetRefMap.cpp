
#include "AssetRefMap.h"

#include "Tools.h"
#include "PlanetWorldSettings.h"

UAssetRefMap::UAssetRefMap():
	Super()
{

}

UAssetRefMap* UAssetRefMap::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetAssetRefMapInstance();
}
