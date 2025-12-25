#include "WebChannelWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "PixelStreamingDelegates.h"
#include "HttpModule.h"
#include "IWebSocket.h"
#include "PropertyAccess.h"
#include "WebSocketsModule.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"

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

	Delegates->OnClosedConnectionNative.AddUObject(
	                                            this,
	                                            &ThisClass::OnClosedConnectionNative
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
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_UpdateSceneElementParamByArea>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_SelectedDevices>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_ClearSelectedDevices>();
		DeserializeStrategiesMap.Add(MessageSPtr->GetCMDName(), MessageSPtr);
	}
	{
		auto MessageSPtr = MakeShared<FMessageBody_Receive_UpdateQueryDeviceToken>();
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

void UWebChannelWorldSystem::OnClosedConnectionNative(
	FString Str,
	FPixelStreamingPlayerId ID,
	bool bIsTrue
	)
{
	ConnetedPlayerIds.Remove(ID);
}

void UWebChannelWorldSystem::NewConnectionNative(
	FString Str,
	FPixelStreamingPlayerId ID,
	bool bIsTrue
	)
{
	ConnetedPlayerIds.Add(ID);
	
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
	
	ConnetedPlayerIds.Empty();
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
	if (JsonStr.FindChar(TEXT('{'), Index) && Index < JsonStr.Len())
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
				auto DeserializeStrategy = DeserializeStrategiesMap[Name];
				
				DeserializeStrategy->Deserialize(JsonStr);
				DeserializeStrategy->DoAction();
				
				DeserializeStrategy->WriteLog();
			}
		}
	}
	else
	{
	}
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UWebChannelWorldSystem::QueryDeviceID(
	const FString& BimID,
	const TFunction<void(bool , const FString&)>& QueryComplete
	)
{
	// 1. URL
	FString QueryDeviceAddress;
	GConfig->GetString(
		TEXT("SmartCitySetting"),
		TEXT("QueryDeviceAddress"),
		QueryDeviceAddress,
		GGameIni
	);
	FString Url = TEXT("http://") + QueryDeviceAddress;

	// 2. data JSON（注意是字符串）
	FString DataJson =
		FString::Printf(
			TEXT("{\"bimId\":\"%s\",\"status\":\"active\"}"),
			*BimID
			);

	// 3. form-urlencoded 内容
	FString PostData = FString::Printf(
		TEXT("service=%s&version=%s&time=%s&token=%s&accessToken=%s&salt=%s&test=%s&osc=%s&data=%s"),
		TEXT("getBimModelDeviceAssociation"),
		TEXT("1.0"),
		TEXT("0"),
		TEXT("token"),
		TEXT("accessToken"),
		TEXT("salt"),
		TEXT("yes"),
		TEXT("test"),
		*FGenericPlatformHttp::UrlEncode(DataJson) // ⚠️ 必须 URL 编码
	);

	// 4. 创建请求
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request =
		FHttpModule::Get().CreateRequest();

	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader(TEXT("user"),UWebChannelWorldSystem::GetInstance()->QueryDeviceToken);
	Request->SetContentAsString(PostData);

	// 5. 回调
	Request->OnProcessRequestComplete().BindLambda(
		[QueryComplete](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
		{
			if (bSuccess && Resp.IsValid())
			{
				// UE_LOG(LogTemp, Log, TEXT("HTTP Code: %d"), Resp->GetResponseCode());
				// UE_LOG(LogTemp, Log, TEXT("Response: %s"), *Resp->GetContentAsString());

				QueryComplete(bSuccess, Resp->GetContentAsString());
			}
			else
			{
				// UE_LOG(LogTemp, Error, TEXT("Request Failed"));

				QueryComplete(bSuccess, TEXT(""));
			}
		}
	);

	// 6. 发送
	Request->ProcessRequest();

	return  Request;
}

void UWebChannelWorldSystem::MessageTickImp()
{
	auto MessageBody_TestSPtr = MakeShared<FMessageBody_UE_Tick>();

	MessageBody_TestSPtr->Text = TEXT("UE PixelStreamer Test");
	MessageBody_TestSPtr->ConnectedID = ConnetedPlayerIds;

	SendMessage(MessageBody_TestSPtr);
}
