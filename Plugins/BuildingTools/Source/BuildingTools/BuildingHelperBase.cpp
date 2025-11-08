
#include "BuildingHelperBase.h"

 void ABuildingHelperBase::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

}

TMap<FString, TSoftObjectPtr<AViewerPawnBase>> ABuildingHelperBase::GetPresetBuildingCameraSeat() const
{
 	return PresetBuildingCameraSeat;
}
