#include "WebChannelWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "PixelStreamingDelegates.h"

#include "Tools.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "PixelStreamingInputComponent.h"
#include "PlanetPlayerController.h"
#include "SceneInteractionDecorator.h"

UWebChannelWorldSystem* UWebChannelWorldSystem::GetInstance()
{
	return Cast<UWebChannelWorldSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(
			GetWorldImp(),
			UWebChannelWorldSystem::StaticClass()
		)
	);
}

void UWebChannelWorldSystem::BindEvent()
{
	UPixelStreamingDelegates* Delegates = UPixelStreamingDelegates::GetPixelStreamingDelegates();

	Delegates->OnAllConnectionsClosed.AddDynamic(this,
	                                             &ThisClass::OnAllConnectionsClosed);

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		PCPtr->PixelStreamingInputPtr->OnInputEvent.AddDynamic(this,
		                                                       &ThisClass::OnInput);
	}
}

void UWebChannelWorldSystem::InitializeDeserializeStrategies()
{
	{
		auto MessageSPtr = MakeShared<FMessageBody_SelectedSpace>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_SelectedDevice>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetName(), MessageSPtr);
	}
}

void UWebChannelWorldSystem::SendMessage(const TSharedPtr<FMessageBody>& Message)
{
	if (Message)
	{
		auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		if (PCPtr)
		{
			PCPtr->PixelStreamingInputPtr->SendPixelStreamingResponse(Message->GetJsonString());
		}
	}
}

void UWebChannelWorldSystem::OnAllConnectionsClosed(FString StreamerId)
{
}

void UWebChannelWorldSystem::OnInput(const FString& Descriptor)
{
	FString JsonStr = Descriptor;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(JsonReader,
	                             jsonObject);

	FString Name;
	if (jsonObject->TryGetStringField(TEXT("Name"),Name))
	{
		if (DeserializeStrategiesMap.Contains(Name))
		{
			DeserializeStrategiesMap[Name]->Deserialize(JsonStr);
		}
	}
}
