#include "SceneInteractionDecorator.h"

#include "SceneElement_AccessControl.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/WebChannelWorldSystem.h"
#include "Components/BoxComponent.h"
#include "Marks/PersonMark.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameOptions.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "SceneInteractionWorldSystem.h"
#include "Algorithm.h"
#include "Elevator.h"
#include "FloorHelper.h"
#include "GameplayCommand.h"
#include "GameplayTagsLibrary.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneElement_PWR_Pipe.h"
#include "TemplateHelper.h"
#include "FloorHelperBase.h"
#include "NavagationPaths.h"

FDecoratorBase::FDecoratorBase(
	FGameplayTag InMainDecoratorType,
	FGameplayTag InBranchDecoratorType
	):
	 MainDecoratorType(InMainDecoratorType)
	 , BranchDecoratorType(InBranchDecoratorType)
{
}

FDecoratorBase::~FDecoratorBase()
{
}

void FDecoratorBase::Entry()
{
}

void FDecoratorBase::Quit()
{
}

bool FDecoratorBase::NeedAsync() const
{
	return false;
}

void FDecoratorBase::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

void FDecoratorBase::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

bool FDecoratorBase::Operation(
	EOperatorType OperatorType
	)
{
	return false;
}

FGameplayTag FDecoratorBase::GetMainDecoratorType() const
{
	return MainDecoratorType;
}

FGameplayTag FDecoratorBase::GetBranchDecoratorType() const
{
	return BranchDecoratorType;
}

void FDecoratorBase::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
}

FEmpty_Decorator::FEmpty_Decorator():
                                    Super(
                                          UGameplayTagsLibrary::Interaction_Mode,
                                          UGameplayTagsLibrary::Interaction_Mode_Empty
                                         )
{
}

void FTour_Decorator::Entry()
{
	Super::Entry();
}

FTour_Decorator::FTour_Decorator():
                                  Super(
                                        UGameplayTagsLibrary::Interaction_Mode,
                                        UGameplayTagsLibrary::Interaction_Mode_Tour
                                       )
{
}

bool FTour_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	PRINTFUNCSTR();

	return Super::Operation(OperatorType);
}

FSceneMode_Decorator::FSceneMode_Decorator():
                                            Super(
                                                  UGameplayTagsLibrary::Interaction_Mode,
                                                  UGameplayTagsLibrary::Interaction_Mode_Scene
                                                 )
{
}

void FSceneMode_Decorator::Entry()
{
	Super::Entry();
}

bool FSceneMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	PRINTFUNCSTR();

	return Super::Operation(OperatorType);
}

FEmergencyMode_Decorator::FEmergencyMode_Decorator():
                                                    Super(
                                                          UGameplayTagsLibrary::Interaction_Mode,
                                                          UGameplayTagsLibrary::Interaction_Mode_Emergency
                                                         )
{
}

void FEmergencyMode_Decorator::Entry()
{
	Super::Entry();

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       UGameplayTagsLibrary::Interaction_Area
			                                      )
		                                     );

	if (!AreaDecoratorSPtr)
	{
		return;
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
		{
			Iter.Value->NavagationPaths->SwitchDisplay(
			                                           AreaDecoratorSPtr->GetCurrentInteraction_Area().MatchesTag(
				                                            Iter.Value->FloorTag
				                                           )
			                                          );
		}
	}
}

void FEmergencyMode_Decorator::Quit()
{
	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
		{
			Iter.Value->NavagationPaths->SwitchDisplay(false);
		}
	}

	Super::Quit();
}

void FEmergencyMode_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (NewDecoratorSPtr)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Area_Floor))
		{
			auto AreaDecoratorSPtr =
				DynamicCastSharedPtr<FArea_Decorator>(NewDecoratorSPtr);

			if (!AreaDecoratorSPtr)
			{
				return;
			}

			for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
			{
				if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
				{
					Iter.Value->NavagationPaths->SwitchDisplay(
															   AreaDecoratorSPtr->GetCurrentInteraction_Area().MatchesTag(
																	Iter.Value->FloorTag
																   )
															  );
				}
			}
		}
		else
		{
			for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
			{
				if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
				{
					Iter.Value->NavagationPaths->SwitchDisplay(false);
				}
			}
		}
	}
}

void FEmergencyMode_Decorator::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorQuit(NewDecoratorSPtr);
}

void FEmergencyMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);
}

FELVRadarMode_Decorator::FELVRadarMode_Decorator():
                                                  Super(
                                                        UGameplayTagsLibrary::Interaction_Mode,
                                                        UGameplayTagsLibrary::Interaction_Mode_ELV_Radar
                                                       )
{
}

FELVRadarMode_Decorator::~FELVRadarMode_Decorator()
{
}

void FELVRadarMode_Decorator::Entry()
{
	Super::Entry();

	GetWorldImp()->GetTimerManager().SetTimer(
	                                          QueryTimerHadnle,
	                                          std::bind(&ThisClass::RadarQuery, this),
#if TEST_RADAR
	                                          1.f,
#else
	                                          UGameOptions::GetInstance()->RadarQueryFrequency,
#endif
	                                          true
	                                         );
}

void FELVRadarMode_Decorator::Quit()
{
	GetWorldImp()->GetTimerManager().ClearTimer(
	                                            QueryTimerHadnle
	                                           );

	for (auto Iter : GeneratedMarkers)
	{
		Iter->Destroy();
	}
	GeneratedMarkers.Empty();

	Super::Quit();
}

bool FELVRadarMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

void FELVRadarMode_Decorator::RadarQuery()
{
#if TEST_RADAR
	QueryComplete();
#else

#endif
}

void FELVRadarMode_Decorator::QueryComplete()
{
#if TEST_RADAR
	int32 Num = FMath::RandRange(1, 10);

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       UGameplayTagsLibrary::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr)
	{
		for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (Iter.Value->FloorTag == AreaDecoratorSPtr->GetCurrentInteraction_Area())
			{
				for (int32 Index = 0; Index < Num; Index++)
				{
					const auto Pt = FMath::RandPointInBox(
					                                      FBox::BuildAABB(
					                                                      Iter.Value->BoxComponentPtr->
					                                                           GetComponentLocation(),
					                                                      Iter.Value->BoxComponentPtr->
					                                                           GetScaledBoxExtent()
					                                                     )
					                                     );
					if (GeneratedMarkers.IsValidIndex(Index))
					{
						GeneratedMarkers[Index]->Update(Pt);
					}
					else
					{
						auto NewMarkPtr = GetWorldImp()->SpawnActor<APersonMark>(
							 UAssetRefMap::GetInstance()->PersonMarkClass
							);
						NewMarkPtr->Update(Pt);

						GeneratedMarkers.Add(NewMarkPtr);
					}
				}

				break;
			}
		}
		for (int32 Index = GeneratedMarkers.Num() - 1; Index >= Num; Index--)
		{
			GeneratedMarkers[Index]->Destroy();
			GeneratedMarkers.RemoveAt(Index);
		}
	}
#else

#endif
}

FPWRMode_Decorator::FPWRMode_Decorator():
                                        Super(
                                              UGameplayTagsLibrary::Interaction_Mode,
                                              UGameplayTagsLibrary::Interaction_Mode_PWR
                                             )
{
}

FPWREnergyMode_Decorator::FPWREnergyMode_Decorator():
                                                    Super(
                                                          UGameplayTagsLibrary::Interaction_Mode,
                                                          UGameplayTagsLibrary::Interaction_Mode_PWR_Energy
                                                         )
{
}

void FPWREnergyMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);

	for (auto Iter : InActors)
	{
		auto PipePtr = Cast<ASceneElement_PWR_Pipe>(Iter);
		if (PipePtr)
		{
			PipeActors.Add(PipePtr);
		}
	}

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       UGameplayTagsLibrary::Interaction_Area
			                                      )
		                                     );

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(AreaDecoratorSPtr->GetBranchDecoratorType());
	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	for (auto Iter : PipeActors)
	{
		if (Iter)
		{
			Iter->SwitchInteractionType(SceneActorConditional);
		}
	}
}

FPWRHVACMode_Decorator::FPWRHVACMode_Decorator():
                                                Super(
                                                      UGameplayTagsLibrary::Interaction_Mode,
                                                      UGameplayTagsLibrary::Interaction_Mode_PWR_HVAC
                                                     )
{
}

FPWRLightingMode_Decorator::FPWRLightingMode_Decorator():
												Super(
													  UGameplayTagsLibrary::Interaction_Mode,
													  UGameplayTagsLibrary::Interaction_Mode_PWR_Lighting
													 )
{
}

FAccessControlMode_Decorator::FAccessControlMode_Decorator():
                                                            Super(
                                                                  UGameplayTagsLibrary::Interaction_Mode,
                                                                  UGameplayTagsLibrary::Interaction_Mode_ELV_AccessControl
                                                                 )
{
}

void FAccessControlMode_Decorator::Entry()
{
	Super::Entry();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ASceneElement_AccessControl::StaticClass(), OutActors);

	USceneInteractionWorldSystem::GetInstance()->ClearFocus();
	USceneInteractionWorldSystem::GetInstance()->ClearRouteMarker();

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(UGameplayTagsLibrary::Interaction_Mode);

	for (auto Iter : OutActors)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(Iter, SceneActorConditional);
	}
}

FElevatorMode_Decorator::FElevatorMode_Decorator():
                                                  Super(
                                                        UGameplayTagsLibrary::Interaction_Mode,
                                                        UGameplayTagsLibrary::Interaction_Mode_Elevator
                                                       )
{
}

void FElevatorMode_Decorator::Entry()
{
	Super::Entry();
}

void FElevatorMode_Decorator::Quit()
{
	Super::Quit();
}

void FElevatorMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);

	
}

FSunShadeMode_Decorator::FSunShadeMode_Decorator():
												  Super(
														UGameplayTagsLibrary::Interaction_Mode,
														UGameplayTagsLibrary::Interaction_Mode_SunShade
													   )
{
}

FArea_Decorator::FArea_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       UGameplayTagsLibrary::Interaction_Area,
	       Interaction_Area
	      )
	 , CurrentInteraction_Area(Interaction_Area)
{
}

void FArea_Decorator::Entry()
{
	Super::Entry();
}

void FArea_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);
}

void FArea_Decorator::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorQuit(NewDecoratorSPtr);
}

FGameplayTag FArea_Decorator::GetCurrentInteraction_Area() const
{
	return CurrentInteraction_Area;
}

void FArea_Decorator::UpdateDisplay()
{
}

void FArea_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
}

FExternalWall_Decorator::FExternalWall_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       Interaction_Area
	      )
{
}

void FExternalWall_Decorator::Entry()
{
	Super::Entry();

	auto DecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetDecorator(
		 UGameplayTagsLibrary::Interaction_Mode
		);
	if (
		DecoratorSPtr
	)
	{
		if (
			DecoratorSPtr->GetMainDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode)
		)
		{
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Empty))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
				
				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
			else if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Emergency))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
				
				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																		  SceneActorConditional,
																		  MulticastDelegate
																		 );

				return;
			}
			else if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Elevator))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
				
				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																		  SceneActorConditional,
																		  MulticastDelegate
																		 );

				return;
			}
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
		}
	}
	else
	{
	}

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
	
	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&

		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          MulticastDelegate
	                                                         );
}

void FExternalWall_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode)
	)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Empty))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																	  SceneActorConditional,
																	  MulticastDelegate
																	 );
			return;
		}
		else if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Emergency))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																	  SceneActorConditional,
																	  MulticastDelegate
																	 );
			return;
		}

		else if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Elevator))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																	  SceneActorConditional,
																	  MulticastDelegate
																	 );
			return;
		}
		else
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());


			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																	  SceneActorConditional,
																	  MulticastDelegate
																	 );
			return;
		}
	}
	else
	{
	}
	
	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&

			
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
															  SceneActorConditional,
															  MulticastDelegate
															 );
}

bool FExternalWall_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	Super::Operation(OperatorType);

	PRINTFUNCSTR();

	switch (OperatorType)
	{
	case EOperatorType::kLeftMouseButton:
	case EOperatorType::kRightMouseButton:
		{
		}
		break;
	case EOperatorType::kNone:
		break;
	default: ;
	}

	return false;
}

FSplitFloor_Decorator::FSplitFloor_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       UGameplayTagsLibrary::Interaction_Area,
	       Interaction_Area
	      )
{
}

void FSplitFloor_Decorator::Entry()
{
	Super::Entry();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_FloorSplit>(
	                                                                    [this](
	                                                                    UGT_FloorSplit* GTPtr
	                                                                    )
	                                                                    {
	                                                                    }
	                                                                   );
}

void FSplitFloor_Decorator::Quit()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_QuitFloorSplit>(
	                                                                        [this](
	                                                                        auto GTPtr
	                                                                        )
	                                                                        {
		                                                                        GTPtr->OnEnd.AddLambda(
			                                                                         [this](
			                                                                         auto
			                                                                         )
			                                                                         {
				                                                                         OnAsyncQuitComplete.
					                                                                         ExecuteIfBound();
			                                                                         }
			                                                                        );
	                                                                        }
	                                                                       );

	Super::Quit();
}

void FSplitFloor_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);
}

bool FSplitFloor_Decorator::NeedAsync() const
{
	return true;
}

bool FSplitFloor_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

FFloor_Decorator::FFloor_Decorator(
	const FGameplayTag& Interaction_Area
	):
	 Super(
	       Interaction_Area
	      )
{
}

FFloor_Decorator::~FFloor_Decorator()
{
}

void FFloor_Decorator::Entry()
{
	Super::Entry();

	auto DecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetDecorator(
		 UGameplayTagsLibrary::Interaction_Mode
		);
	if (
		DecoratorSPtr
	)
	{
		if (
			DecoratorSPtr->GetMainDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode)
		)
		{
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Empty))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
				
				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
			else if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Elevator))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
				
				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
			else if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Emergency))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
				
				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
			else
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&

					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
		}
	}
	else
	{
	}

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&

		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          MulticastDelegate
	                                                         );
}

void FFloor_Decorator::Quit()
{
	USceneInteractionWorldSystem::GetInstance()->ClearFocus();
	USceneInteractionWorldSystem::GetInstance()->ClearRouteMarker();

	Super::Quit();
}

void FFloor_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode)
	)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Empty))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          MulticastDelegate
			                                                         );
			return;
		}
		else if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Elevator))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																	  SceneActorConditional,
																	  MulticastDelegate
																	 );
			return;
		}
		else if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(UGameplayTagsLibrary::Interaction_Mode_Emergency))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
																	  SceneActorConditional,
																	  MulticastDelegate
																	 );
			return;
		}
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());


			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&

				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          MulticastDelegate
			                                                         );
			return;
		}
	}
	else
	{
	}
	
	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&

			
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
															  SceneActorConditional,
															  MulticastDelegate
															 );
}

bool FFloor_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	Super::Operation(OperatorType);

	switch (OperatorType)
	{
	case EOperatorType::kLeftMouseButton:
	case EOperatorType::kRightMouseButton:
		{
			TArray<struct FHitResult> OutHits;

			auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));

			FVector2D MousePosition;
			PCPtr->GetMousePosition(
			                        MousePosition.X,
			                        MousePosition.Y
			                       );

			FVector WorldLocation;
			FVector WorldDirection;
			PCPtr->DeprojectScreenPositionToWorld(
			                                      MousePosition.X,
			                                      MousePosition.Y,
			                                      WorldLocation,
			                                      WorldDirection
			                                     );

			// 优先检测设备
			{
				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(Device_Object);
				GetWorldImp()->LineTraceMultiByObjectType(
				                                          OutHits,
				                                          WorldLocation,
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
				                                                           ->LinetraceDistance),
				                                          ObjectQueryParams
				                                         );

				for (const auto& Iter : OutHits)
				{
					if (Iter.GetActor())
					{
						if (Iter.GetActor()->IsHidden())
						{
							continue;
						}

						USceneInteractionWorldSystem::GetInstance()->ClearFocus();

						FSceneElementConditional SceneActorConditional;

						SceneActorConditional.ConditionalSet.AddTag(UGameplayTagsLibrary::Interaction_Mode_View);

						USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
							 Iter.GetActor(),
							 SceneActorConditional
							);


						auto MessageBodySPtr = MakeShared<FMessageBody_SelectedDevice>();

						MessageBodySPtr->DeviceID = TEXT("");

						UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);

						return true;
					}
				}
			}

			// 检测区域
			{
				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(Space_Object);
				GetWorldImp()->LineTraceMultiByObjectType(
				                                          OutHits,
				                                          WorldLocation,
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
				                                                           ->LinetraceDistance),
				                                          ObjectQueryParams
				                                         );

				for (const auto& Iter : OutHits)
				{
					if (Iter.GetActor())
					{
						if (Iter.GetActor()->IsHidden())
						{
							continue;
						}

						USceneInteractionWorldSystem::GetInstance()->ClearFocus();

						FSceneElementConditional SceneActorConditional;

						SceneActorConditional.ConditionalSet.AddTag(UGameplayTagsLibrary::Interaction_Mode_Focus);

						USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
							 Iter.GetActor(),
							 SceneActorConditional
							);

						PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformLocaterBySpace>(
							 [&Iter](
							 UGT_CameraTransformLocaterBySpace* GTPtr
							 )
							 {
								 if (GTPtr)
								 {
									 GTPtr->SpaceActorPtr = Iter.GetActor();
								 }
							 }
							);

						auto MessageBodySPtr = MakeShared<FMessageBody_SelectedSpace>();

						MessageBodySPtr->SpaceName = TEXT("");

						UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);
						return true;
					}
				}
			}
		}
		break;
	case EOperatorType::kNone:
		break;
	default: ;
	}

	USceneInteractionWorldSystem::GetInstance()->ClearFocus();

	return false;
}

void FFloor_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);

	auto Result = UKismetAlgorithm::GetCameraSeat(
	                                              InActors,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.ViewRot,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.FOV
	                                             );

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ModifyCameraTransform>(
		 [Result](
		 UGT_ModifyCameraTransform* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->TargetLocation = Result.Key.GetLocation();
				 GTPtr->TargetRotation = Result.Key.GetRotation().Rotator();
				 GTPtr->TargetTargetArmLength = Result.Value;
			 }
		 }
		);
}
