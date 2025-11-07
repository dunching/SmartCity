#include "FloorHelper.h"

#include "Components/BoxComponent.h"
#include "Components/RectLightComponent.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "DatasmithSceneActor.h"
#include "Dynamic_SkyBase.h"
#include "FloorHelper_Description.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewerPawnBase.h"
#include "WeatherSystem.h"

AFloorHelper::AFloorHelper(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	FloorHelper_DescriptionAttachTransform = CreateDefaultSubobject<USceneComponent>(
		 TEXT("FloorHelper_DescriptionAttachTransform")
		);
	FloorHelper_DescriptionAttachTransform->SetMobility(EComponentMobility::Movable);

	FloorHelper_DescriptionAttachTransform->SetupAttachment(BoxComponentPtr);
}

void AFloorHelper::BeginPlay()
{
	Super::BeginPlay();

	auto Handle = UWeatherSystem::GetInstance()->GetDynamicSky()->OnHourChanged.AddCallback(
		 std::bind(
		           &ThisClass::OnHourChanged,
		           this,
		           std::placeholders::_1
		          )
		);
	Handle->bIsAutoUnregister = false;
}

void AFloorHelper::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);

	const auto Box = BoxComponentPtr->GetLocalBounds();
	FloorHelper_DescriptionAttachTransform->SetRelativeLocation(FVector(-Box.BoxExtent.X, -Box.BoxExtent.Y, 0));
	FloorHelper_DescriptionAttachTransform->SetRelativeRotation(FRotator(0, 180, 0));
}

void AFloorHelper::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			OnExternalWall();
			if (FloorHelper_DescriptionPtr)
			{
			}
			else
			{
				if (bDisplayDescritpion)
				{
					FActorSpawnParameters SpawnParameters;

					SpawnParameters.CustomPreSpawnInitalization = [this](
						auto ActorPtr
						)
						{
							auto FloorPtr = Cast<AFloorHelper_Description>(ActorPtr);
							if (FloorPtr)
							{
								FloorPtr->SetFloor(this);
							}
						};
					FloorHelper_DescriptionPtr = GetWorld()->SpawnActor<AFloorHelper_Description>(
						 UAssetRefMap::GetInstance()->FloorHelper_DescriptionClass,
						 SpawnParameters
						);
				}
			}
			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			if (FloorHelper_DescriptionPtr)
			{
				FloorHelper_DescriptionPtr->Destroy();
			}

			FloorHelper_DescriptionPtr = nullptr;

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor)
		)
		{
			if (FloorHelper_DescriptionPtr)
			{
				FloorHelper_DescriptionPtr->Destroy();
			}

			FloorHelper_DescriptionPtr = nullptr;

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			if (FloorHelper_DescriptionPtr)
			{
				FloorHelper_DescriptionPtr->Destroy();
			}
			FloorHelper_DescriptionPtr = nullptr;

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		if (FloorHelper_DescriptionPtr)
		{
			FloorHelper_DescriptionPtr->Destroy();
		}
		FloorHelper_DescriptionPtr = nullptr;

		return;
	}
}

void AFloorHelper_Computer::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);

	for (auto Iter : PresetBuildingCameraSeat)
	{
		auto ActorPtr = Iter.Value.LoadSynchronous();
		if (ActorPtr)
		{
			ActorPtr->Destroy();
		}
	}
	PresetBuildingCameraSeat.Empty();

	const auto BoxPt = BoxComponentPtr->GetComponentLocation();

	for (const auto& DatasmithSceneActorIter : AllReference.SoftDecorationItem.DatasmithSceneActorSet)
	{
		TArray<AActor*> OutActors;
		DatasmithSceneActorIter->GetAttachedActors(OutActors, true, true);
		for (auto Iter : OutActors)
		{
			if (Iter)
			{
				auto Components = Iter->GetComponents();
				for (auto SecondIter : Components)
				{
					auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
					if (!InterfacePtr)
					{
						continue;
					}
					auto AUDPtr = Cast<UDatasmithAssetUserData>(
					                                            InterfacePtr->GetAssetUserDataOfClass(
						                                             UDatasmithAssetUserData::StaticClass()
						                                            )
					                                           );
					if (!AUDPtr)
					{
						continue;
					}

					auto Name = AUDPtr->MetaData.Find(TEXT("服务器"));
					if (!Name)
					{
						continue;
					}

					auto STCPtr = Cast<UStaticMeshComponent>(SecondIter);
					if (!STCPtr)
					{
						continue;
					}

					const auto STCTransform = STCPtr->GetComponentTransform();

					FVector Min;
					FVector Max;
					STCPtr->GetLocalBounds(Min, Max);
					FBox Bounds(Min, Max);

					const auto Pt1 = STCTransform.TransformPosition(
					                                                Bounds.GetCenter() + FVector(
						                                                 0,
						                                                 Bounds.GetExtent().Y,
						                                                 0
						                                                )
					                                               );
					const auto Pt2 = STCTransform.TransformPosition(
					                                                Bounds.GetCenter() - FVector(
						                                                 0,
						                                                 Bounds.GetExtent().Y,
						                                                 0
						                                                )
					                                               );

					DrawDebugSphere(GetWorld(), Pt1,20,20,FColor::Red, false, 10);
					DrawDebugSphere(GetWorld(), Pt2,20,20,FColor::Yellow, false, 10);
					
					if (FVector::Distance(BoxPt, Pt1) > FVector::Distance(BoxPt, Pt2))
					{
						auto ViewerPawnPtr = GetWorld()->SpawnActor<AViewerPawnBase>(
							 ViewerPawnClass,
							 Pt1,
							 STCPtr->GetComponentRotation() + FRotator(0, -90, 0)
							);
						PresetBuildingCameraSeat.Add(*Name, ViewerPawnPtr);
					}
					else
					{
						auto ViewerPawnPtr = GetWorld()->SpawnActor<AViewerPawnBase>(
							 ViewerPawnClass,
							 Pt2,
							 STCPtr->GetComponentRotation() + FRotator(0, -90, 0) + FRotator(0, 180, 0)
							);

						PresetBuildingCameraSeat.Add(*Name, ViewerPawnPtr);
					}

					break;
				}
			}
		}
	}
}

ABuilding_Floor_Mask::ABuilding_Floor_Mask(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void ABuilding_Floor_Mask::SetFloor(
	AFloorHelper* FloorPtr
	)
{
	if (FloorPtr)
	{
		FVector Origin;
		FVector BoxExtent;

		FloorPtr->GetActorBounds(false, Origin, BoxExtent);

		SetActorLocation(Origin - FVector(0, 0, BoxExtent.Z));
	}
}

void AFloorHelper::OnHourChanged(
	int32 Hour
	)
{
	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );

	if (!AreaDecoratorSPtr)
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(true);
		}
		return;
	}

	if (!AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_ExternalWall))
	{
		for (auto Iter : RectLightComponentAry)
		{
			Iter->SetHiddenInGame(true);
		}
		return;
	}

	if (Hour > 18 || Hour < 8)
	{
		for (auto Iter : RectLightComponentAry)
		{
			// Iter->SetHiddenInGame(false	);
		}
		return;
	}
	for (auto Iter : RectLightComponentAry)
	{
		Iter->SetHiddenInGame(true);
	}
}

void AFloorHelper::OnExternalWall()
{
	const auto Hour = UWeatherSystem::GetInstance()->GetDynamicSky()->GetCurrentHour();

	if (Hour > 18 || Hour < 8)
	{
		for (auto Iter : RectLightComponentAry)
		{
			// Iter->SetHiddenInGame(false	);
		}
		return;
	}
	for (auto Iter : RectLightComponentAry)
	{
		Iter->SetHiddenInGame(true);
	}
}
