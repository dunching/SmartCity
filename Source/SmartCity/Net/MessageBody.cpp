#include "MessageBody.h"

#include "GameFramework/InputSettings.h"

#include "AssetRefMap.h"
#include "DatasmithSceneActor.h"
#include "FloorHelper.h"
#include "FloorHelper_Description.h"
#include "GameOptions.h"
#include "IPSSI.h"
#include "PlanetPlayerCameraManager.h"
#include "SceneElement_DeviceBase.h"
#include "SceneElement_RadarMode.h"
#include "SceneElement_Space.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewTowerProcessor.h"
#include "ViewSingleDeviceProcessor.h"
#include "ViewSingleFloorProcessor.h"
#include "TourPawn.h"
#include "ViewerPawnBase.h"

FMessageBody::FMessageBody()
{
	Guid = FGuid::NewGuid();
}

FMessageBody::~FMessageBody()
{
}

TSharedPtr<FJsonObject> FMessageBody_Send::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = MakeShareable<FJsonObject>(new FJsonObject);

	RootJsonObj->SetStringField(
	                            CMD,
	                            CMD_Name
	                           );

	RootJsonObj->SetStringField(
	                            TEXT("Guid"),
	                            Guid.ToString()
	                           );

	return RootJsonObj;
}

void FMessageBody_Receive::Deserialize(
	const FString& JsonStr
	)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	FString GuidStr;
	if (jsonObject->TryGetStringField(TEXT("Guid"), GuidStr))
	{
		Guid = FGuid(JsonStr);
	}
}

void FMessageBody_Receive::DoAction() const
{
}

FMessageBody_Receive_SwitchViewArea::FMessageBody_Receive_SwitchViewArea()
{
	CMD_Name = TEXT("SwitchViewArea");
}

void FMessageBody_Receive_SwitchViewArea::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	FString AreaTagStr;
	if (jsonObject->TryGetStringField(TEXT("AreaTag"), AreaTagStr))
	{
		AreaTag = FGameplayTag::RequestGameplayTag(*AreaTagStr);
	}
}

void FMessageBody_Receive_SwitchViewArea::DoAction() const
{
	USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(AreaTag);
}

FMessageBody_Receive_SwitchMode::FMessageBody_Receive_SwitchMode()
{
	CMD_Name = TEXT("SwitchMode");
}

void FMessageBody_Receive_SwitchMode::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	FString Str;
	if (jsonObject->TryGetStringField(TEXT("Mode"), Str))
	{
		ModeTag = FGameplayTag::RequestGameplayTag(*Str);
	}
}

void FMessageBody_Receive_SwitchMode::DoAction() const
{
	USceneInteractionWorldSystem::GetInstance()->SwitchInteractionMode(ModeTag);
}

FMessageBody_Receive_AdjustHour::FMessageBody_Receive_AdjustHour()
{
	CMD_Name = TEXT("AdjustHour");
}

void FMessageBody_Receive_AdjustHour::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetNumberField(TEXT("Hour"), Hour))
	{
	}
}

void FMessageBody_Receive_AdjustHour::DoAction() const
{
	// 确认当前的模式
	auto DecoratorSPtr =
		DynamicCastSharedPtr<FInteraction_Decorator>(
		                                             USceneInteractionWorldSystem::GetInstance()->
		                                             GetDecorator(
		                                                          USmartCitySuiteTags::Interaction_Interaction
		                                                         )
		                                            );
	if (DecoratorSPtr)
	{
		DecoratorSPtr->SetCurrentHour(Hour);
	}
}

FMessageBody_Receive_AdjustWeather::FMessageBody_Receive_AdjustWeather()
{
	CMD_Name = TEXT("AdjustWeather");
}

void FMessageBody_Receive_AdjustWeather::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	FString Str;
	if (jsonObject->TryGetStringField(TEXT("Weather"), Str))
	{
		Weather = FGameplayTag::RequestGameplayTag(*(TEXT("Weather.") + Str));
	}
}

void FMessageBody_Receive_AdjustWeather::DoAction() const
{
	// 确认当前的模式
	auto DecoratorSPtr =
		DynamicCastSharedPtr<FInteraction_Decorator>(
		                                             USceneInteractionWorldSystem::GetInstance()->
		                                             GetDecorator(
		                                                          USmartCitySuiteTags::Interaction_Interaction
		                                                         )
		                                            );
	if (DecoratorSPtr)
	{
		DecoratorSPtr->SetCurrentWeather(Weather);
	}
}

FMessageBody_Receive_UpdateViewConfig::FMessageBody_Receive_UpdateViewConfig()
{
	CMD_Name = TEXT("UpdateViewConfig");
}

void FMessageBody_Receive_UpdateViewConfig::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetNumberField(TEXT("WallTranlucent"), ViewConfig.WallTranlucent))
	{
	}

	if (jsonObject->TryGetNumberField(TEXT("PillarTranlucent"), ViewConfig.PillarTranlucent))
	{
	}

	if (jsonObject->TryGetNumberField(TEXT("StairsTranlucent"), ViewConfig.StairsTranlucent))
	{
	}

	if (jsonObject->TryGetBoolField(TEXT("ShowCurtainWall"), ViewConfig.bShowCurtainWall))
	{
	}

	if (jsonObject->TryGetBoolField(TEXT("ShowFurniture"), ViewConfig.bShowFurniture))
	{
	}

	if (jsonObject->TryGetBoolField(TEXT("ImmediatelyUpdate"), bImmediatelyUpdate))
	{
	}
}

void FMessageBody_Receive_UpdateViewConfig::DoAction() const
{
	// 确认当前的模式
	auto DecoratorSPtr =
		DynamicCastSharedPtr<FInteraction_Decorator>(
		                                             USceneInteractionWorldSystem::GetInstance()->
		                                             GetDecorator(
		                                                          USmartCitySuiteTags::Interaction_Interaction
		                                                         )
		                                            );
	if (DecoratorSPtr)
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  SPtr->UpdateViewConfig(ViewConfig);
		                                                                  },
		                                                                  bImmediatelyUpdate
		                                                                 );
	}
}

FMessageBody_Receive_LocaterDeviceByID::FMessageBody_Receive_LocaterDeviceByID()
{
	CMD_Name = TEXT("LocaterDeviceByID");
}

void FMessageBody_Receive_LocaterDeviceByID::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetStringField(TEXT("DeviceID"), DeviceID))
	{
	}
}

void FMessageBody_Receive_LocaterDeviceByID::DoAction() const
{
	auto SceneElementPtr = USceneInteractionWorldSystem::GetInstance()->FindSceneActor(DeviceID);
	if (SceneElementPtr.IsValid())
	{
		auto DevicePtr = Cast<ASceneElement_DeviceBase>(SceneElementPtr.Get());
		if (DevicePtr && DevicePtr->BelongFloor)
		{
			USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
			                                                                   USmartCitySuiteTags::Interaction_Area_ViewDevice,
			                                                                   [DevicePtr, this](
			                                                                   const TSharedPtr<FDecoratorBase>&
			                                                                   AreaDecoratorSPtr
			                                                                   )
			                                                                   {
				                                                                   auto SpaceAreaDecoratorSPtr =
					                                                                   DynamicCastSharedPtr<
						                                                                   FViewDevice_Decorator>(
						                                                                    AreaDecoratorSPtr
						                                                                   );
				                                                                   if (SpaceAreaDecoratorSPtr)
				                                                                   {
					                                                                   SpaceAreaDecoratorSPtr->
						                                                                   SceneElementPtr = DevicePtr;
				                                                                   }
			                                                                   }
			                                                                  );
		}
	}
}

FMessageBody_Receive_LocaterSpaceByID::FMessageBody_Receive_LocaterSpaceByID()
{
	CMD_Name = TEXT("LocaterSpaceByID");
}

void FMessageBody_Receive_LocaterSpaceByID::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetStringField(TEXT("SpaceID"), SpaceID))
	{
	}

	FString FloorStr;
	if (jsonObject->TryGetStringField(TEXT("Floor"), FloorStr))
	{
		Floor = FGameplayTag::RequestGameplayTag(*FloorStr);
	}
}

void FMessageBody_Receive_LocaterSpaceByID::DoAction() const
{
	Super::DoAction();

	if (UAssetRefMap::GetInstance()->FloorHelpers.Contains(Floor))
	{
		auto FloorRef = UAssetRefMap::GetInstance()->FloorHelpers[Floor];
		auto FloorPtr = FloorRef.LoadSynchronous();

		for (auto Iter : FloorPtr->AllReference.SpaceItemSet.DatasmithSceneActorSet)
		{
			TArray<AActor*> OutActors;
			Iter->GetAttachedActors(OutActors, true, true);

			for (auto SpaceIter : OutActors)
			{
				auto SpacePtr = Cast<ASceneElement_Space>(SpaceIter);
				if (SpacePtr && SpacePtr->Category == SpaceID)
				{
					USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
						 USmartCitySuiteTags::Interaction_Area_Space,
						 [SpacePtr, this](
						 const TSharedPtr<FDecoratorBase>& AreaDecoratorSPtr
						 )
						 {
							 auto SpaceAreaDecoratorSPtr = DynamicCastSharedPtr<
								 FViewSpace_Decorator>(AreaDecoratorSPtr);
							 if (SpaceAreaDecoratorSPtr)
							 {
								 SpaceAreaDecoratorSPtr->Floor = Floor;
								 SpaceAreaDecoratorSPtr->SceneElementPtr = SpacePtr;
							 }
						 }
						);

					return;
				}
			}
		}
	}
}

FMessageBody_Receive_SwitchInteractionType::FMessageBody_Receive_SwitchInteractionType()
{
	CMD_Name = TEXT("SwitchInteractionType");
}

void FMessageBody_Receive_SwitchInteractionType::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetStringField(TEXT("InteractionType"), InteractionType))
	{
	}
}

void FMessageBody_Receive_SwitchInteractionType::DoAction() const
{
	// 确认当前的模式
	auto DecoratorSPtr =
		DynamicCastSharedPtr<FInteraction_Decorator>(
		                                             USceneInteractionWorldSystem::GetInstance()->
		                                             GetDecorator(
		                                                          USmartCitySuiteTags::Interaction_Interaction
		                                                         )
		                                            );
	if (DecoratorSPtr)
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this](
		                                                                  const TSharedPtr<FInteraction_Decorator>&
		                                                                  DecoratorSPtr
		                                                                  )
		                                                                  {
			                                                                  auto ControllConfig = DecoratorSPtr->
				                                                                  GetConfigControlConfig();
			                                                                  if (InteractionType == TEXT("Device"))
			                                                                  {
				                                                                  ControllConfig.InteractionType =
					                                                                  EInteractionType::kDevice;
			                                                                  }
			                                                                  else if (InteractionType == TEXT("Space"))
			                                                                  {
				                                                                  ControllConfig.InteractionType =
					                                                                  EInteractionType::kSpace;
			                                                                  }
			                                                                  else
			                                                                  {
				                                                                  ControllConfig.InteractionType =
					                                                                  EInteractionType::kSpace;
			                                                                  }
			                                                                  DecoratorSPtr->UpdateControlConfig(
				                                                                   ControllConfig
				                                                                  );
		                                                                  },
		                                                                  true
		                                                                 );
	}
}

FMessageBody_Receive_UpdateRadarInfo::FMessageBody_Receive_UpdateRadarInfo()
{
	CMD_Name = TEXT("UpdateRadarInfo");
}

void FMessageBody_Receive_UpdateRadarInfo::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetStringField(TEXT("SpaceID"), DeviceID))
	{
	}

	Value.Empty();

	const TArray<TSharedPtr<FJsonValue>>* OutArray;
	if (jsonObject->TryGetArrayField(TEXT("value"), OutArray))
	{
		for (const auto& JsonValueIter : *OutArray)
		{
			auto ObjSPtr = JsonValueIter->AsObject();

			FUpdateRadarInfo UpdateRadarInfo;

			ObjSPtr->TryGetNumberField(TEXT("accX"), UpdateRadarInfo.Acceleration.X);
			ObjSPtr->TryGetNumberField(TEXT("accY"), UpdateRadarInfo.Acceleration.Y);

			ObjSPtr->TryGetNumberField(TEXT("posX"), UpdateRadarInfo.Position.X);
			ObjSPtr->TryGetNumberField(TEXT("posY"), UpdateRadarInfo.Position.Y);

			ObjSPtr->TryGetNumberField(TEXT("velX"), UpdateRadarInfo.Velocity.X);
			ObjSPtr->TryGetNumberField(TEXT("velY"), UpdateRadarInfo.Velocity.Y);

			ObjSPtr->TryGetNumberField(TEXT("ec"), UpdateRadarInfo.EC);
			ObjSPtr->TryGetNumberField(TEXT("g"), UpdateRadarInfo.G);

			ObjSPtr->TryGetNumberField(TEXT("numberOfTargets"), UpdateRadarInfo.NumberOfTargets);

			ObjSPtr->TryGetStringField(TEXT("tid"), UpdateRadarInfo.TID);

			Value.Add(UpdateRadarInfo);
		}
	}
}

void FMessageBody_Receive_UpdateRadarInfo::DoAction() const
{
	Super::DoAction();

	auto SceneElement_RadarModePtr = Cast<ASceneElement_RadarMode>(
	                                                               USceneInteractionWorldSystem::GetInstance()->
	                                                               FindSceneActor(DeviceID)
	                                                              );

	if (SceneElement_RadarModePtr)
	{
		TMap<FString, FVector> Pts;

		for (const auto& Iter : Value)
		{
			// 单位转换 
			Pts.Add(Iter.TID, FVector(Iter.Position.X, Iter.Position.Y, 0) * 100);
		}

		SceneElement_RadarModePtr->UpdatePositions(Pts);
	}
}

FMessageBody_SelectedFloor::FMessageBody_SelectedFloor()
{
	CMD_Name = TEXT("SelectedFloor");
}

TSharedPtr<FJsonObject> FMessageBody_SelectedFloor::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = Super::SerializeBody();

	TArray<TSharedPtr<FJsonValue>> Array;

	for (const auto& Iter : SpacesMap)
	{
		auto SpaceValue = Iter.Key->GetSceneElementData();
		auto SpaceObj = SpaceValue->AsObject();

		TArray<TSharedPtr<FJsonValue>> DeviceArray;

		for (const auto& SecondIter : Iter.Value)
		{
			DeviceArray.Add(SecondIter->GetSceneElementData());
		}

		SpaceObj->SetArrayField(TEXT("Devices"), DeviceArray);

		Array.Add(SpaceValue);
	}

	RootJsonObj->SetArrayField(
	                           TEXT("Spaces"),
	                           Array
	                          );

	if (FloorHelper)
	{
		RootJsonObj->SetStringField(
		                            TEXT("FloorTag"),
		                            FloorHelper->FloorTag.ToString()
		                           );

		RootJsonObj->SetStringField(
		                            TEXT("Floor"),
		                            FloorHelper->FloorIndexDescription
		                           );

		for (const auto& Iter : FloorHelper->PresetBuildingCameraSeat)
		{
			auto PresetValueSPtr = MakeShared<FJsonValueString>(Iter.Key);

			Array.Add(PresetValueSPtr);
		}

		RootJsonObj->SetArrayField(
		                           TEXT("Spaces"),
		                           Array
		                          );
	}

	return RootJsonObj;
}

FMessageBody_SelectedSpace::FMessageBody_SelectedSpace()
{
	CMD_Name = TEXT("SelectedSpace");
}

TSharedPtr<FJsonObject> FMessageBody_SelectedSpace::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = Super::SerializeBody();

	TArray<TSharedPtr<FJsonValue>> Array;

	for (const auto Iter : DeviceIDAry)
	{
		TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject);

		JsonObj->SetStringField(
		                        TEXT("Type"),
		                        Iter.Type
		                       );

		JsonObj->SetStringField(
		                        TEXT("ID"),
		                        Iter.DeviceID
		                       );

		auto DeviceObjSPtr = MakeShared<FJsonValueObject>(JsonObj);

		Array.Add(DeviceObjSPtr);
	}

	RootJsonObj->SetArrayField(
	                           TEXT("DeviceIDAry"),
	                           Array
	                          );

	return RootJsonObj;
}

FMessageBody_SelectedDevice::FMessageBody_SelectedDevice()
{
	CMD_Name = TEXT("SelectedDevice");
}

TSharedPtr<FJsonObject> FMessageBody_SelectedDevice::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = Super::SerializeBody();

	TArray<TSharedPtr<FJsonValue>> Array;

	for (const auto Iter : DeviceIDAry)
	{
		Array.Add(MakeShared<FJsonValueString>(Iter));
	}

	RootJsonObj->SetArrayField(
	                           TEXT("DeviceIDAry"),
	                           Array
	                          );

	return RootJsonObj;
}

FMessageBody_ViewDevice::FMessageBody_ViewDevice()
{
	CMD_Name = TEXT("ViewDevice");
}

FMessageBody_Receive_AdjustCameraSeat::FMessageBody_Receive_AdjustCameraSeat()
{
	CMD_Name = TEXT("AdjustCameraSeat");
}

void FMessageBody_Receive_AdjustCameraSeat::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetBoolField(TEXT("UseClampPitch"), bUseClampPitch))
	{
	}
	
	if (jsonObject->TryGetNumberField(TEXT("MinPitch"), MinPitch))
	{
	}

	if (jsonObject->TryGetNumberField(TEXT("MaxPitch"), MaxPitch))
	{
	}

	if (jsonObject->TryGetBoolField(TEXT("AllowRotByYaw"), bAllowRotByYaw))
	{
	}
}

void FMessageBody_Receive_AdjustCameraSeat::DoAction() const
{
	UGameOptions::GetInstance()->bAllowRotByYaw = bAllowRotByYaw;

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
	
	auto ConfigControlConfig = DecoratorSPtr->GetConfigControlConfig();
	ConfigControlConfig.bUseCustomPitchLimit = bUseClampPitch;
	ConfigControlConfig.ViewPitchMin = MinPitch;
	ConfigControlConfig.ViewPitchMax = MaxPitch;
	DecoratorSPtr->UpdateControlConfig(ConfigControlConfig);
	
	{
		auto ViewBuildingProcessorSPtr = DynamicCastSharedPtr<TourProcessor::FViewTowerProcessor>(
			 UInputProcessorSubSystem_Imp::GetInstance()->GetCurrentAction()
			);
		if (ViewBuildingProcessorSPtr)
		{
			ViewBuildingProcessorSPtr->AdjustCameraSeat(FRotator(MinPitch, 0, 0));
			return;
		}
	}
	{
		Cast<APlanetPlayerCameraManager>(
		                                 GEngine->GetFirstLocalPlayerController(GetWorldImp())->PlayerCameraManager
		                                )->UpdateCameraSetting();

		return;
	}
}

TSharedPtr<FJsonObject> FMessageBody_ViewDevice::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = Super::SerializeBody();

	RootJsonObj->SetStringField(
	                            TEXT("Type"),
	                            Type
	                           );

	RootJsonObj->SetStringField(
	                            TEXT("SceneElementID"),
	                            DeviceID
	                           );

	return RootJsonObj;
}

FMessageBody_ViewConfigChanged::FMessageBody_ViewConfigChanged()
{
	CMD_Name = TEXT("ViewConfigChanged");
}

TSharedPtr<FJsonObject> FMessageBody_ViewConfigChanged::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = Super::SerializeBody();

	RootJsonObj->SetNumberField(
	                            TEXT("WallTranlucent"),
	                            ViewConfig.WallTranlucent
	                           );

	RootJsonObj->SetNumberField(
	                            TEXT("PillarTranlucent"),
	                            ViewConfig.PillarTranlucent
	                           );

	RootJsonObj->SetNumberField(
	                            TEXT("StairsTranlucent"),
	                            ViewConfig.StairsTranlucent
	                           );

	RootJsonObj->SetBoolField(
	                          TEXT("ShowCurtainWall"),
	                          ViewConfig.bShowCurtainWall
	                         );

	RootJsonObj->SetBoolField(
	                          TEXT("ShowFurniture"),
	                          ViewConfig.bShowFurniture
	                         );

	return RootJsonObj;
}

FMessageBody_Receive_UpdateFloorDescription::FMessageBody_Receive_UpdateFloorDescription()
{
	CMD_Name = TEXT("UpdateFloorDescription");
}

void FMessageBody_Receive_UpdateFloorDescription::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );


	FString FloorStr;
	if (jsonObject->TryGetStringField(TEXT("FloorSet"), FloorStr))
	{
		Floor = FGameplayTag::RequestGameplayTag(*FloorStr);
	}

	if (jsonObject->TryGetStringField(TEXT("FloorDescription"), FloorDescription))
	{
	}
}

void FMessageBody_Receive_UpdateFloorDescription::DoAction() const
{
	Super::DoAction();

	if (UAssetRefMap::GetInstance()->FloorHelpers.Contains(Floor))
	{
		auto FloorRef = UAssetRefMap::GetInstance()->FloorHelpers[Floor];
		auto FloorPtr = FloorRef.LoadSynchronous();

		FloorPtr->FloorHelper_DescriptionPtr->UpdateFloorDescription(FloorDescription);
	}
}

FMessageBody_Receive_ViewSpeacialArea::FMessageBody_Receive_ViewSpeacialArea()
{
	CMD_Name = TEXT("ViewSpeacialArea");
}

void FMessageBody_Receive_ViewSpeacialArea::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	FloorSet.Empty();
	const TArray<TSharedPtr<FJsonValue>>* OutArray = nullptr;
	if (jsonObject->TryGetArrayField(TEXT("FloorSet"), OutArray))
	{
		FString FloorStr;
		for (const auto JsonValue : *OutArray)
		{
			FloorStr = JsonValue->AsString();
			FloorSet.Add(FGameplayTag::RequestGameplayTag(*FloorStr));
		}
	}

	if (jsonObject->TryGetStringField(TEXT("Seat"), Seat))
	{
	}
}

void FMessageBody_Receive_ViewSpeacialArea::DoAction() const
{
	Super::DoAction();

	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (Iter.Value->PresetBuildingCameraSeat.Contains(Seat))
		{
			auto ViewerPawnBasePtr = Iter.Value->PresetBuildingCameraSeat[
				Seat];
			USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
			                                                                   USmartCitySuiteTags::Interaction_Area_SpecialArea,
			                                                                   [this, ViewerPawnBasePtr](
			                                                                   const TSharedPtr<FDecoratorBase>&
			                                                                   AreaDecoratorSPtr
			                                                                   )
			                                                                   {
				                                                                   auto SpaceAreaDecoratorSPtr =
					                                                                   DynamicCastSharedPtr<
						                                                                   FViewSpecialArea_Decorator>(
						                                                                    AreaDecoratorSPtr
						                                                                   );
				                                                                   if (SpaceAreaDecoratorSPtr)
				                                                                   {
					                                                                   SpaceAreaDecoratorSPtr->
						                                                                   ViewerPawnBasePtr =
						                                                                   ViewerPawnBasePtr.
						                                                                   LoadSynchronous();

					                                                                   SpaceAreaDecoratorSPtr->
						                                                                   FloorSet = FloorSet;
				                                                                   }
			                                                                   }
			                                                                  );

			return;
		}
	}
}

FMessageBody_UE_Initialized::FMessageBody_UE_Initialized()
{
	CMD_Name = TEXT("UE_Initialized");
}

TSharedPtr<FJsonObject> FMessageBody_UE_Initialized::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = Super::SerializeBody();

	RootJsonObj->SetBoolField(
	                          TEXT("IsOK"),
	                          true
	                         );

	return RootJsonObj;
}

FMessageBody_Test::FMessageBody_Test()
{
}

FMessageBody_Receive_SetRelativeTransoform::FMessageBody_Receive_SetRelativeTransoform()
{
	CMD_Name = TEXT("SetRelativeTransoform");
}

void FMessageBody_Receive_SetRelativeTransoform::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetStringField(TEXT("DeviceID"), DeviceID))
	{
	}

	FRotator Rotator;

	if (jsonObject->TryGetNumberField(TEXT("Rotation_Pitch"), Rotator.Pitch))
	{
	}

	if (jsonObject->TryGetNumberField(TEXT("Rotation_Yaw"), Rotator.Yaw))
	{
	}

	if (jsonObject->TryGetNumberField(TEXT("Rotation_Roll"), Rotator.Roll))
	{
	}

	Transform.SetRotation(Rotator.Quaternion());

	FVector Location;

	if (jsonObject->TryGetNumberField(TEXT("Translation_X"), Location.X))
	{
	}

	if (jsonObject->TryGetNumberField(TEXT("Translation_Y"), Location.Y))
	{
	}

	if (jsonObject->TryGetNumberField(TEXT("Translation_Z"), Location.Z))
	{
	}

	Transform.SetTranslation(Location);
}

void FMessageBody_Receive_SetRelativeTransoform::DoAction() const
{
	Super::DoAction();

	auto SceneElementPtr = USceneInteractionWorldSystem::GetInstance()->FindSceneActor(DeviceID);
	if (SceneElementPtr.IsValid())
	{
		auto DevicePtr = Cast<ASceneElement_DeviceBase>(SceneElementPtr);
		if (DevicePtr)
		{
			DevicePtr->UpdateReletiveTransform(Transform);
		}
	}
}

FMessageBody_Receive_UpdateSceneElementParam::FMessageBody_Receive_UpdateSceneElementParam()
{
	CMD_Name = TEXT("UpdateSceneElementParam");
}

void FMessageBody_Receive_UpdateSceneElementParam::Deserialize(
	const FString& JsonStr
	)
{
	Super::Deserialize(JsonStr);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject->TryGetBoolField(TEXT("ImmediatelyUpdate"), bImmediatelyUpdate))
	{
	}

	const TSharedPtr<FJsonObject>* OutObject = nullptr;
	if (jsonObject->TryGetObjectField(TEXT("SceneElements"), OutObject))
	{
		for (auto Iter : (*OutObject)->Values)
		{
			auto& Ref = ExtensionParamMap.Add(Iter.Key, {});

			const auto ObjSPtr =  Iter.Value->AsObject();
			for (const auto& SecondIter : ObjSPtr->Values)
			{
				Ref.Add(SecondIter.Key, SecondIter.Value->AsString());
			}
		}
	}
}

void FMessageBody_Receive_UpdateSceneElementParam::DoAction() const
{
	Super::DoAction();

	for (const auto& Iter : ExtensionParamMap)
	{
		auto SceneElementPtr =
			USceneInteractionWorldSystem::GetInstance()->FindSceneActor(Iter.Key);

		if (SceneElementPtr.IsValid())
		{
			SceneElementPtr->UpdateExtensionParamMap(Iter.Value, bImmediatelyUpdate);
		}
	}
}

FString FMessageBody_Send::GetJsonString() const
{
	TSharedPtr<FJsonObject> RootJsonObj = SerializeBody();

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(
	                           RootJsonObj.ToSharedRef(),
	                           Writer
	                          );

	return JsonString;
}

FString FMessageBody::CMD = TEXT("CMD");

FString FMessageBody::GetCMDName() const
{
	return CMD_Name;
}
