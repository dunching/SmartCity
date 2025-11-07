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
		auto MessageSPtr = MakeShared<FMessageBody_Receive_AdjustCameraSeat>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_SwitchViewArea>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_SwitchMode>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_AdjustHour>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_AdjustWeather>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_UpdateViewConfig>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_LocaterDeviceByID>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_SwitchInteractionType>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_UpdateRadarInfo>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_SetRelativeTransoform>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_LocaterSpaceByID>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_UpdateFloorDescription>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_ViewSpeacialArea>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_UpdateSceneElementParam>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
}

void UWebChannelWorldSystem::SendMessage(
	const TSharedPtr<FMessageBody_Send>& Message
	)
{
	if (Message)
	{
		const auto JsonStr = Message->GetJsonString();

		if (JsonStr.IsEmpty())
		{
			return;
		}

		TArray<FString> JsonStrAry;

		const auto MessageSplitNumber = UGameOptions::GetInstance()->MessageSplitNumber;
		for (int32 Index = 0; Index < JsonStr.Len();)
		{
			if (Index + MessageSplitNumber >= JsonStr.Len())
			{
				const auto Str = JsonStr.Mid(Index, JsonStr.Len() - Index);

				JsonStrAry.Add(Str);

				break;
			}
			else
			{
				const auto Str = JsonStr.Mid(Index, MessageSplitNumber);

				JsonStrAry.Add(Str);

				Index += MessageSplitNumber;
			}
		}

		struct FMessageHelper
		{
			TArray<FString> StrAry;

			int32 Index = 0;
		};

		TSharedPtr<FMessageHelper> MessageHelperSPtr = MakeShared<FMessageHelper>();

		FGuid Guid = FGuid::NewGuid();

		for (int32 Index = 0; Index < JsonStrAry.Num(); Index++)
		{
			TSharedPtr<FJsonObject> RootJsonObj = MakeShareable<FJsonObject>(new FJsonObject);

			RootJsonObj->SetStringField(
			                            TEXT("GUID"),
			                            Guid.ToString()
			                           );

			RootJsonObj->SetNumberField(
			                            TEXT("CurrentIndex"),
			                            Index
			                           );

			RootJsonObj->SetNumberField(
			                            TEXT("Total"),
			                            JsonStrAry.Num()
			                           );

			RootJsonObj->SetStringField(
			                            TEXT("CurrentStr"),
			                            JsonStrAry[Index]
			                           );

			FString JsonString;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
			FJsonSerializer::Serialize(
			                           RootJsonObj.ToSharedRef(),
			                           Writer
			                          );
			MessageHelperSPtr->StrAry.Add(JsonString);
		}

		FTickerDelegate TickerDelegate;

		TickerDelegate.BindLambda(
		                          [this](
		                          float,
		                          const TSharedPtr<FMessageHelper>& NewMessageHelperSPtr
		                          )
		                          {
			                          auto PCPtr = Cast<APlanetPlayerController>(
				                           GEngine->GetFirstLocalPlayerController(GetWorld())
				                          );
			                          if (PCPtr)
			                          {
				                          PCPtr->PixelStreamingInputPtr->SendPixelStreamingResponse(
					                           NewMessageHelperSPtr->StrAry[NewMessageHelperSPtr->Index]
					                          );
				                          NewMessageHelperSPtr->Index++;
				                          if (NewMessageHelperSPtr->Index >= NewMessageHelperSPtr->StrAry.Num())
				                          {
					                          return false;
				                          }
			                          }
			                          return true;
		                          },
		                          MessageHelperSPtr
		                         );

		FTSTicker::GetCoreTicker().AddTicker(TickerDelegate);
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
		                                       1.f,
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

	int32 Index = -1;
	if (JsonStr.FindLastChar(TEXT('}'), Index) && (Index + 1) < JsonStr.Len())
	{
		JsonStr.RemoveAt(Index + 1, JsonStr.Len() - Index + 1);
	}
	if (JsonStr.FindLastChar(TEXT('{'), Index) && Index < JsonStr.Len())
	{
		JsonStr.RemoveAt(0, Index);
	}

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject)
	{
		FString Name;
		if (jsonObject->TryGetStringField(FMessageBody_Receive::CMD, Name))
		{
			if (DeserializeStrategiesMap.Contains(Name))
			{
				DeserializeStrategiesMap[Name]->Deserialize(JsonStr);
				DeserializeStrategiesMap[Name]->DoAction();
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

	// SendMessage(MessageBody_TestSPtr);
}
