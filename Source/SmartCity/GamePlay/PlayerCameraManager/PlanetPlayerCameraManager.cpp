#include "PlanetPlayerCameraManager.h"

#include "SceneElement_AccessControl.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"

static TAutoConsoleVariable<int32> APlanetPlayerCameraManager_Draw(
                                                                   TEXT("APlanetPlayerCameraManager.Draw"),
                                                                   0,
                                                                   TEXT("")
                                                                   TEXT(" default: 0")
                                                                  );

APlanetPlayerCameraManager::APlanetPlayerCameraManager(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bClientSimulatingViewTarget = 1;

	// 往下看的限制
	ViewPitchMin = -70;

	// 往上看的限制
	ViewPitchMax = 50;
}

void APlanetPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
}

void APlanetPlayerCameraManager::UpdateCamera(
	float DeltaTime
	)
{
	Super::UpdateCamera(DeltaTime);

#ifdef WITH_EDITOR
	if (APlanetPlayerCameraManager_Draw.GetValueOnGameThread())
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			const FMinimalViewInfo& CurrentPOV = GetCameraCacheView();

			DrawDebugLine(
			              GetWorld(),
			              CurrentPOV.Location,
			              CurrentPOV.Location + (CurrentPOV.Rotation.Vector() * 500),
			              FColor::Yellow,
			              false,
			              3
			             );
		}
	}
#endif
}

void APlanetPlayerCameraManager::UpdateCameraSetting()
{
	auto DecoratorSPtr =
		DynamicCastSharedPtr<FInteraction_Decorator>(
													 USceneInteractionWorldSystem::GetInstance()->
													 GetDecorator(
																  USmartCitySuiteTags::Interaction_Interaction
																 )
													);

	if (!DecoratorSPtr)
	{
		return;
	}

	const auto ConfigControlConfig = DecoratorSPtr->GetConfigControlConfig();
	if (DecoratorSPtr->GetConfigControlConfig().bUseCustomPitchLimit)
	{
		// 往下看的限制
		ViewPitchMin = ConfigControlConfig.ViewPitchMin;

		// 往上看的限制
		ViewPitchMax = ConfigControlConfig.ViewPitchMax;
	}
	else
	{
		// 往下看的限制
		ViewPitchMin = ConfigControlConfig.ProcessPitchMin;

		// 往上看的限制
		ViewPitchMax = ConfigControlConfig.ProcessPitchMax;
	}
}
