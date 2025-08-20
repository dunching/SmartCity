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

	Delegates->OnConnectedToSignallingServerNative.AddUObject(
	                                                          this,
	                                                          &ThisClass::OnConnectedToSignallingServerNative
	                                                         );

	Delegates->OnNewConnectionNative.AddUObject(
	                                            this,
	                                            &ThisClass::NewConnectionNative
	                                           );

	Delegates->OnAllConnectionsClosed.AddDynamic(
	                                             this,
	                                             &ThisClass::OnAllConnectionsClosed
	                                            );

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (PCPtr)
	{
		PCPtr->PixelStreamingInputPtr->OnInputEvent.AddDynamic(
		                                                       this,
		                                                       &ThisClass::OnInput
		                                                      );
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

void UWebChannelWorldSystem::SendMessage(
	const TSharedPtr<FMessageBody>& Message
	)
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

void UWebChannelWorldSystem::OnConnectedToSignallingServerNative()
{
}

void UWebChannelWorldSystem::NewConnectionNative(
	FString Str,
	FPixelStreamingPlayerId ID,
	bool bIsTrue
	)
{
	if (MessageTickTimerHandle.IsValid())
	{
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
		                                       MessageTickTimerHandle,
		                                       this,
		                                       &ThisClass::MessageTickImp,
		                                       1.f / 30,
		                                       true
		                                      );
	}
}

void UWebChannelWorldSystem::OnAllConnectionsClosed(
	FString StreamerId
	)
{
	GetWorld()->GetTimerManager().ClearTimer(MessageTickTimerHandle);
}

void UWebChannelWorldSystem::OnInput(
	const FString& Descriptor
	)
{
	FString JsonStr = Descriptor;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject)
	{
		FString Name;
		if (jsonObject->TryGetStringField(TEXT("Name"), Name))
		{
			if (DeserializeStrategiesMap.Contains(Name))
			{
				DeserializeStrategiesMap[Name]->Deserialize(JsonStr);
			}
		}
	}
	else
	{
	}
}

void UWebChannelWorldSystem::MessageTickImp()
{
	auto MessageBody_TestSPtr = MakeShared<FMessageBody_Test>();

	MessageBody_TestSPtr->Text = TEXT("UE PixelStreamer Test");
	
	SendMessage(MessageBody_TestSPtr);
}
