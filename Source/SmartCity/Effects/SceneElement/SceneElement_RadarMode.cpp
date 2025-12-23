#include "SceneElement_RadarMode.h"

#include <string>

#include "Marks/PersonMark.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "IWebSocket.h"
#include "Kismet/KismetStringLibrary.h"
#include "HttpModule.h"
#include "WebSocketsModule.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "FloorHelper.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "SceneElement_RadarSweep.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "WebChannelWorldSystem.h"

struct RadarPoint
{
	float x;
	float y;
	uint8_t range;
	int8_t angle;
};

struct RadarTarget
{
	uint32_t tid;
	float posX;
	float posY;
	float velX;
	float velY;
	float accX;
	float accY;
	float g;
};

struct RadarFrameResult
{
	uint32_t frameNumber;
	std::vector<RadarPoint> points;
	std::vector<RadarTarget> targets;
	uint32_t checksum;
	bool isValid = false;
};

constexpr uint32_t TLV_TYPE_POINT_CLOUD_2D_ZIP = 10;
constexpr uint32_t TLV_TYPE_TARGET_LIST_2D = 7;
constexpr uint8_t SYNC_BYTES[8] = {2, 1, 4, 3, 6, 5, 8, 7};

uint32_t ReadU32(
	const std::vector<uint8_t>& buf,
	size_t off
	)
{
	return *(uint32_t*)(&buf[off]);
}

uint16_t ReadU16(
	const std::vector<uint8_t>& buf,
	size_t off
	)
{
	return *(uint16_t*)(&buf[off]);
}

float ReadF32(
	const std::vector<uint8_t>& buf,
	size_t off
	)
{
	return *(float*)(&buf[off]);
}

void ParsePointCloudZipTLV(
	const std::vector<uint8_t>& buf,
	size_t offset,
	uint32_t length,
	std::vector<RadarPoint>& points
	)
{
	if (length < 20)
		return;

	float rangeRes = ReadF32(buf, offset + 0);
	float angleRes = ReadF32(buf, offset + 4);
	float dopplerRes = ReadF32(buf, offset + 8);

	size_t ffp = 12;
	int pointNum = (length - 20) / 4;

	if (pointNum > 250)
		return;

	for (int i = 0; i < pointNum; ++i)
	{
		size_t p = offset + ffp;
		ffp += 4;

		uint8_t range = buf[p];
		int8_t angle = (int8_t)buf[p + 1];

		float trueRange = range * rangeRes;
		float trueAngle = angle * angleRes;

		float x = trueRange * std::sin(trueAngle);
		float y = trueRange * std::cos(trueAngle);

		if (std::isfinite(x) && std::isfinite(y))
		{
			points.push_back({x, y, range, angle});
		}
	}
}

void ParseTargetListTLV(
	const std::vector<uint8_t>& buf,
	size_t offset,
	uint32_t length,
	uint16_t checksum,
	std::vector<RadarTarget>& targets
	)
{
	uint32_t size = (checksum == 55) ? 112 : 68;

	int targetNum = (length - 8) / size;
	if (targetNum <= 0 || targetNum > 20)
		return;

	size_t ffp = 0;

	for (int i = 0; i < targetNum; ++i)
	{
		size_t base = offset + ffp;
		if (base + size > buf.size())
			break;

		size_t gOffset = (size == 112) ? 108 : 64;

		RadarTarget t;
		t.tid = ReadU32(buf, base + 0);
		t.posX = ReadF32(buf, base + 4);
		t.posY = ReadF32(buf, base + 8);
		t.velX = ReadF32(buf, base + 12);
		t.velY = ReadF32(buf, base + 16);
		t.accX = ReadF32(buf, base + 20);
		t.accY = ReadF32(buf, base + 24);
		t.g = ReadF32(buf, base + gOffset);

		if (std::isfinite(t.posX) && std::isfinite(t.posY))
		{
			targets.push_back(t);
		}

		ffp += size;
	}
}

RadarFrameResult ParseRadarDataFromHex(
	const std::string& hex
	)
{
	RadarFrameResult result;

	// 1️⃣ hex → byte buffer
	std::vector<uint8_t> buf;
	buf.reserve(hex.size() / 2);

	for (size_t i = 0; i < hex.size(); i += 2)
	{
		buf.push_back(
		              static_cast<uint8_t>(
			              std::stoi(hex.substr(i, 2), nullptr, 16))
		             );
	}

	// 2️⃣ 查找同步字节
	size_t syncOffset = SIZE_MAX;
	for (size_t i = 0; i + 8 <= buf.size(); ++i)
	{
		if (memcmp(&buf[i], SYNC_BYTES, 8) == 0)
		{
			syncOffset = i;
			break;
		}
	}
	if (syncOffset == SIZE_MAX)
		return result;

	// 3️⃣ 帧头
	size_t header = syncOffset + 8;
	if (header + 44 > buf.size())
		return result;

	result.frameNumber = ReadU32(buf, header + 16);
	uint16_t numTLVs = ReadU16(buf, header + 40);
	result.checksum = ReadU16(buf, header + 42);

	// 4️⃣ TLV
	size_t tlvOffset = header + 44;

	for (uint16_t i = 0; i < numTLVs && tlvOffset + 8 <= buf.size(); ++i)
	{
		uint32_t type = ReadU32(buf, tlvOffset);
		uint32_t length = ReadU32(buf, tlvOffset + 4);

		if (length <= 0 || tlvOffset + length > buf.size())
		{
			tlvOffset += 8;
			continue;
		}

		if (type == TLV_TYPE_POINT_CLOUD_2D_ZIP)
		{
			ParsePointCloudZipTLV(buf, tlvOffset + 8, length, result.points);
		}
		else if (type == TLV_TYPE_TARGET_LIST_2D)
		{
			ParseTargetListTLV(
			                   buf,
			                   tlvOffset + 8,
			                   length,
			                   result.checksum,
			                   result.targets
			                  );
		}

		tlvOffset += length;
	}

	result.isValid = true;
	return result;
}

ASceneElement_RadarMode::ASceneElement_RadarMode(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RelativeTransformComponent);

	StaticMeshComponent->SetRelativeRotation(FRotator(0, 90, 0));

	NetState_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NetState_StaticMeshComponent"));
	NetState_StaticMeshComponent->SetupAttachment(RootComponent);

	NetState_StaticMeshComponent->SetRelativeRotation(FRotator(0, 90, 0));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 24;

	IntervalTime = 10.f;
}

void ASceneElement_RadarMode::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void ASceneElement_RadarMode::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_RadarMode::Tick(
	float DeltaTime
	)
{
	Super::Tick(DeltaTime);

	const auto Pt = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();
	auto Dir = (Pt - NetState_StaticMeshComponent->GetComponentLocation()).GetSafeNormal();
	auto Rot = Dir.Rotation();

	Rot.Roll = 0.f;
	Rot.Pitch = 0.f;
	Rot.Yaw += 90.f;
	
	NetState_StaticMeshComponent->SetWorldRotation(Rot);
}

void ASceneElement_RadarMode::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	GetWorldTimerManager().ClearTimer(ClearTimerHandle);

	Super::EndPlay(EndPlayReason);
}

FBox ASceneElement_RadarMode::GetComponentsBoundingBox(
	bool bNonColliding,
	bool bIncludeFromChildActors
	) const
{
	FBox Box(ForceInit);

	ForEachComponent<UPrimitiveComponent>(
	                                      bIncludeFromChildActors,
	                                      [&](
	                                      const UPrimitiveComponent* InPrimComp
	                                      )
	                                      {
		                                      // Only use collidable components to find collision bounding box.
		                                      if (InPrimComp->IsRegistered() && (
			                                          bNonColliding || InPrimComp->IsCollisionEnabled()))
		                                      {
			                                      Box += InPrimComp->Bounds.GetBox();
		                                      }
	                                      }
	                                     );

	return Box;
}

void ASceneElement_RadarMode::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	TArray<FVector> Pts;

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr)
	{
		const auto FloorBox = BelongFloor->BoxComponentPtr->CalcBounds(
		                                                               BelongFloor->BoxComponentPtr->
		                                                               GetComponentToWorld()
		                                                              );
		const auto Offset = FloorBox.GetBox().GetExtent().Z;
		for (int i = 0; i < 5; ++i)
		{
			const auto Pt = FMath::RandPointInBox(
			                                      FBox(
			                                           FVector((Deepth * 2), -(Deepth * 8), 0),
			                                           FVector((Deepth * 8), -(Deepth * 2), 0)
			                                          )
			                                     );
			Pts.Add(Pt);
		}

		// UpdatePositions( Pts);
	}

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			QuitAllState();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_BatchControl)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Interaction)
		)
		{
			EntryShoweviceEffect();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			(ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
			 ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)) &&
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode)
		)
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if ((ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) ||
		     ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Space)))
		{
			EntryShowDevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_View)
		)
		{
			EntryViewDevice();

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_Focus)
		)
		{
			EntryShowDevice();

			return;
		}
	}

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		QuitAllState();

		return;
	}
}

void ASceneElement_RadarMode::EntryFocusDevice()
{
	Super::EntryFocusDevice();

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(true);
		StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}

	auto MessageBodySPtr = MakeShared<FMessageBody_ViewDevice>();

	MessageBodySPtr->DeviceID = SceneElementID;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageBodySPtr);

	Connect();
}

void ASceneElement_RadarMode::EntryViewDevice()
{
	Super::EntryViewDevice();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(true);
		StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}

	SweepActor->SetActorHiddenInGame(false);

	SweepActor->StaticMeshComponent->SetRenderCustomDepth(true);
	SweepActor->StaticMeshComponent->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);

	Connect();
}

void ASceneElement_RadarMode::EntryShowDevice()
{
	Super::EntryShowDevice();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	SweepActor->SetActorHiddenInGame(true);
	SweepActor->StaticMeshComponent->SetRenderCustomDepth(false);

	for (auto Iter : GeneratedMarkers)
	{
		if (Iter.Value)
		{
			Iter.Value->Destroy();
		}
	}
	GeneratedMarkers.Empty();

	Connect();
}

void ASceneElement_RadarMode::EntryShoweviceEffect()
{
	Super::EntryShoweviceEffect();

	SetActorHiddenInGame(false);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	SweepActor->SetActorHiddenInGame(false);
	SweepActor->StaticMeshComponent->SetRenderCustomDepth(false);

	Connect();
}

void ASceneElement_RadarMode::QuitAllState()
{
	Super::QuitAllState();

	SetActorHiddenInGame(true);

	SweepActor->SetActorHiddenInGame(true);

	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}

	ClearMarks();

	Close();
}

void ASceneElement_RadarMode::SetNetState(
	ENetState NetState
	)
{
	switch (NetState)
	{
	case ENetState::kOnLine:
		break;
	case ENetState::kOffLine:
		break;
	case ENetState::kQueryFailed:
		break;
	}
}

void ASceneElement_RadarMode::ClearMarks()
{
	for (auto Iter : GeneratedMarkers)
	{
		if (Iter.Value)
		{
			Iter.Value->Destroy();
		}
	}
	GeneratedMarkers.Empty();
}

void ASceneElement_RadarMode::InitialSocket()
{
	// FApp::SetBenchmarking(true);
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::LoadModuleChecked<FWebSocketsModule>("WebSockets");
	}

	// 如果已存在连接先关掉
	if (Socket.IsValid())
	{
		Socket->Close();
		Socket.Reset();
	}

	// 可传协议：CreateWebSocket(Url, TEXT("")); // 或 "ws"/"wss" 的子协议
	FString QueryRadarWSAddress;
	GConfig->GetString(
	                   TEXT("SmartCitySetting"),
	                   TEXT("QueryRadarWSAddress"),
	                   QueryRadarWSAddress,
	                   GGameIni
	                  );
	FString Url = TEXT("ws://") + QueryRadarWSAddress;
	Socket = FWebSocketsModule::Get().CreateWebSocket(Url);

	BindEvents();
}

void ASceneElement_RadarMode::Connect()
{
	if (Socket)
	{
	}
	else
	{
		InitialSocket();
	}

	if (Socket.IsValid())
	{
		if (Socket->IsConnected())
		{
		}
		else
		{
			Socket->Connect();
		}

		Socket->Send(FString::Printf(TEXT("0406,%s"), *DeviceRealID));
	}
}

void ASceneElement_RadarMode::SendText(
	const FString& Message
	)
{
	if (Socket.IsValid() && Socket->IsConnected())
	{
		Socket->Send(Message);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[WS] Not connected, send ignored"));
	}
}

void ASceneElement_RadarMode::Close(
	int32 Code,
	const FString& Reason
	)
{
	if (Socket.IsValid())
	{
		Socket->Close(Code, Reason);
		Socket.Reset();
	}
}

void ASceneElement_RadarMode::BindEvents()
{
	if (!Socket.IsValid())
	{
		return;
	}

	Socket->OnConnected().AddLambda(
	                                [this]()
	                                {
		                                ReconnectTry = 0;
		                                UE_LOG(LogTemp, Log, TEXT("[WS] Connected"));
		                                // 连接后可发送鉴权/订阅等
		                                // SendText(TEXT("{\"type\":\"hello\"}"));
	                                }
	                               );

	Socket->OnConnectionError().AddLambda(
	                                      [this](
	                                      const FString& Error
	                                      )
	                                      {
		                                      UE_LOG(LogTemp, Error, TEXT("[WS] ConnectionError: %s"), *Error);
		                                      // ScheduleReconnect();
	                                      }
	                                     );

	Socket->OnClosed().AddLambda(
	                             [this](
	                             int32 StatusCode,
	                             const FString& Reason,
	                             bool bWasClean
	                             )
	                             {
		                             UE_LOG(
		                                    LogTemp,
		                                    Warning,
		                                    TEXT("[WS] Closed code=%d clean=%d reason=%s"),
		                                    StatusCode,
		                                    bWasClean,
		                                    *Reason
		                                   );
		                             // 非正常关闭就重连（按你业务调整）
		                             if (!bWasClean)
		                             {
			                             // ScheduleReconnect();
		                             }
	                             }
	                            );

	Socket->OnMessage().AddLambda(
	                              [this](
	                              const FString& Message
	                              )
	                              {
		                              RadarFrameResult R = ParseRadarDataFromHex(TCHAR_TO_UTF8(*Message));

		                              // 这里解析 JSON：FJsonSerializer / UE::Json 等

		                              TMap<FString, FVector> Pts;
		                              for (const auto& Iter : R.targets)
		                              {
			                              FVector Pt(-Iter.posX, Iter.posY, 0.f);
			                              Pts.Add(UKismetStringLibrary::Conv_IntToString(Iter.tid), Pt * 100);

			                              PRINTINVOKEWITHSTR(FString::Printf(TEXT("%d %s"),Iter.tid, *Pt.ToString()));
		                              }
		                              UpdatePositions(Pts);
	                              }
	                             );

	Socket->OnMessageSent().AddLambda(
	                                  [](
	                                  const FString& Message
	                                  )
	                                  {
		                                  UE_LOG(LogTemp, Log, TEXT("[WS] Sent: %s"), *Message);
	                                  }
	                                 );

	// 二进制消息（如果服务器推的是 bytes）
	// Socket->OnRawMessage().AddLambda(
	//                                  [](
	//                                  const void* Data,
	//                                  SIZE_T Size,
	//                                  SIZE_T BytesRemaining
	//                                  )
	//                                  {
	// 	                                 UE_LOG(
	// 	                                        LogTemp,
	// 	                                        Log,
	// 	                                        TEXT("[WS] RawMessage size=%d remaining=%d"),
	// 	                                        (int32)Size,
	// 	                                        (int32)BytesRemaining
	// 	                                       );
	//                                  }
	//                                 );
}

void ASceneElement_RadarMode::QueryDeviceInfoComplete(
	bool bSuccess,
	const FString& ResponStr
	)
{
	Super::QueryDeviceInfoComplete(bSuccess, ResponStr);

	SetNetState(ENetState::kQueryFailed);

	if (DeviceRealID.IsEmpty())
	{
		Close();
	}
	else
	{
		Connect();
	}
}

void ASceneElement_RadarMode::ScheduleReconnect()
{
	// 没有 World 就不重连（比如在非游戏线程/生命周期不对）
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ReconnectTry++;
	const float Delay = FMath::Clamp(1.0f * ReconnectTry, 1.0f, 10.0f); // 简单线性退避
	UE_LOG(LogTemp, Warning, TEXT("[WS] Reconnect in %.1fs (try %d)"), Delay, ReconnectTry);

	World->GetTimerManager().ClearTimer(ReconnectTimer);
	World->GetTimerManager().SetTimer(
	                                  ReconnectTimer,
	                                  [this]()
	                                  {
		                                  if (!LastUrl.IsEmpty())
		                                  {
			                                  Connect();
		                                  }
	                                  },
	                                  Delay,
	                                  false
	                                 );
}

void ASceneElement_RadarMode::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			auto InterfacePtr = Cast<IInterface_AssetUserData>(STPtr->GetStaticMeshComponent());
			if (!InterfacePtr)
			{
				return;
			}
			auto AUDPtr = Cast<UDatasmithAssetUserData>(
			                                            InterfacePtr->GetAssetUserDataOfClass(
				                                             UDatasmithAssetUserData::StaticClass()
				                                            )
			                                           );

			CheckIsJiaCeng(AUDPtr);

			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());

			for (int32 Index = 0; Index < STPtr->GetStaticMeshComponent()->GetNumMaterials(); Index++)
			{
				StaticMeshComponent->SetMaterial(Index, STPtr->GetStaticMeshComponent()->GetMaterial(Index));
			}

			UpdateCollisionBox({StaticMeshComponent});

			const auto Box = CollisionComponentHelper->GetScaledBoxExtent();
			CollisionComponentHelper->SetBoxExtent(Box + FVector(5));

			SweepActor = GetWorld()->SpawnActor<ASceneElement_RadarSweep>(GetActorLocation(), GetActorRotation());
			SweepActor->StaticMeshComponent->SetStaticMesh(SweepMesh.LoadSynchronous());
			SweepActor->StaticMeshComponent->SetRelativeRotation(FRotator(0, 180, 0));
			const auto MatsNum = SweepActor->StaticMeshComponent->GetNumMaterials();
			for (int32 Index = 0; Index < MatsNum; Index++)
			{
				SweepActor->StaticMeshComponent->SetMaterial(Index, SweepMatInst.LoadSynchronous());
			}

			const auto Scale = Deepth / 10;
			SweepActor->StaticMeshComponent->SetRelativeScale3D(FVector(Scale, Scale, 1));

			SweepActor->StaticMeshComponent->SetCastShadow(false);
			SweepActor->StaticMeshComponent->SetReceivesDecals(false);
		}
	}
}

void ASceneElement_RadarMode::InitialSceneElement()
{
	Super::InitialSceneElement();

	SetNetState(ENetState::kQueryFailed);

	const auto Extent = BelongFloor->BoxComponentPtr->GetScaledBoxExtent();
	const auto Pt = BelongFloor->BoxComponentPtr->GetComponentLocation();
	auto NewPt = NetState_StaticMeshComponent->GetComponentLocation();

	NewPt.Z = Pt.Z + Extent.Z + 20;

	NetState_StaticMeshComponent->SetWorldLocation(NewPt);
}

void ASceneElement_RadarMode::UpdateReletiveTransform(
	const FTransform& NewRelativeTransform
	)
{
	Super::UpdateReletiveTransform(NewRelativeTransform);

	SweepActor->SetActorLocation(RelativeTransformComponent->GetComponentLocation());

	auto Rot = RelativeTransformComponent->GetComponentRotation();

	Rot.Pitch = 0;
	Rot.Roll = 0;

	SweepActor->SetActorRotation(Rot);
}

void ASceneElement_RadarMode::UpdatePositions(
	const TMap<FString, FVector>& Pts
	)
{
	{
		if (
			CurrentConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			CurrentConditionalSet.ConditionalSet.HasTagExact(
			                                                 USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar
			                                                )
		)
		{
		}
		else if (
			CurrentConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor) &&
			CurrentConditionalSet.ConditionalSet.HasTagExact(
			                                                 USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception
			                                                )
		)
		{
		}
		else if (
			CurrentConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Mode_View)
		)
		{
		}
		else
		{
		}
	}

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr)
	{
		const auto FloorBox = BelongFloor->BoxComponentPtr->CalcBounds(
		                                                               BelongFloor->BoxComponentPtr->
		                                                               GetComponentToWorld()
		                                                              );
		const auto Offset = FloorBox.GetBox().GetExtent().Z;

		for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (
				Iter.Value->GameplayTagContainer.HasTag(AreaDecoratorSPtr->GetCurrentInteraction_Area()) ||
				Iter.Value->GameplayTagContainer.HasTag(BelongFloor->FloorTag)
			)
			{
				const auto FloorLocation = Iter.Value->GetActorLocation();

				auto Marks = MakeShared<TSet<APersonMark*>>();


				for (const auto& PtIter : Pts)
				{
					auto Pt = PtIter.Value;

					Pt.Z = -Offset + 80;

					if (GeneratedMarkers.Contains(PtIter.Key))
					{
						GeneratedMarkers[PtIter.Key]->Update(Pt);

						GeneratedMarkers[PtIter.Key]->Marks = Marks;

						Marks->Add(GeneratedMarkers[PtIter.Key]);
					}
					else
					{
						auto NewMarkPtr = GetWorldImp()->SpawnActor<APersonMark>(
							 UAssetRefMap::GetInstance()->PersonMarkClass
							);
						NewMarkPtr->AttachToComponent(
						                              RelativeTransformComponent,
						                              FAttachmentTransformRules::KeepRelativeTransform
						                             );
						NewMarkPtr->SetStartPt(Pt);

						NewMarkPtr->Marks = Marks;

						Marks->Add(NewMarkPtr);

						GeneratedMarkers.Add(PtIter.Key, NewMarkPtr);
					}
				}

				SetNetState(ENetState::kOnLine);

				GetWorldTimerManager().SetTimer(
				                                ClearTimerHandle,
				                                FTimerDelegate::CreateLambda(
				                                                             [this]()
				                                                             {
					                                                             SetNetState(ENetState::kOffLine);

					                                                             ClearMarks();
				                                                             }
				                                                            ),
				                                2.f,
				                                false
				                               );

				for (auto PtIter = GeneratedMarkers.CreateIterator(); PtIter; ++PtIter)
				{
					if (Pts.Contains(PtIter->Key))
					{
					}
					else
					{
						PtIter->Value->Destroy();
						PtIter.RemoveCurrent();
					}
				}

				return;
			}
		}
	}
}
