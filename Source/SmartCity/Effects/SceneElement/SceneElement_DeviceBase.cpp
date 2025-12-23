#include "SceneElement_DeviceBase.h"

#include "ActorSequenceComponent.h"
#include "DatasmithAssetUserData.h"
#include "Components/BoxComponent.h"
#include "Interfaces/IHttpRequest.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "MessageBody.h"
#include "RouteMarker.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"


ASceneElement_DeviceBase::ASceneElement_DeviceBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RelativeTransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RelativeTransformComponent"));
	RelativeTransformComponent->SetupAttachment(RootComponent);

	CollisionComponentHelper = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponentHelper"));
	CollisionComponentHelper->SetupAttachment(RootComponent);

	CollisionComponentHelper->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponentHelper->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponentHelper->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);
	CollisionComponentHelper->SetCollisionResponseToChannel(Device_Object, ECollisionResponse::ECR_Overlap);
	CollisionComponentHelper->SetCollisionObjectType(Device_Object);
}

void ASceneElement_DeviceBase::UpdateReletiveTransform(
	const FTransform& NewRelativeTransform
	)
{
	RelativeTransformComponent->SetRelativeTransform(NewRelativeTransform);
}

void ASceneElement_DeviceBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_DeviceBase::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	if (Request.IsValid())
	{
		Request->CancelRequest();
	}

	GetWorldTimerManager().ClearTimer(QueryDeviceTimerHandel);

	Super::EndPlay(EndPlayReason);
}

void ASceneElement_DeviceBase::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	Super::ReplaceImp(ActorPtr, InUserData);

	DeviceTypeStr = InUserData.Value;
}

void ASceneElement_DeviceBase::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData,
	const TMap<FName, FString>& NewUserData
	)
{
	Super::Merge(ActorRef, InUserData, NewUserData);

	DeviceTypeStr = InUserData.Value;
}

void ASceneElement_DeviceBase::BeginInteraction()
{
	Super::BeginInteraction();

	bIsOpened = true;
}

void ASceneElement_DeviceBase::EndInteraction()
{
	Super::EndInteraction();

	bIsOpened = false;
}

void ASceneElement_DeviceBase::InitialSceneElement()
{
	Super::InitialSceneElement();

	if (BelongFloor)
	{
		return;
	}

	auto ParentPtr = GetAttachParentActor();
	AFloorHelper* FloorPtr = nullptr;
	for (; ParentPtr;)
	{
		ParentPtr = ParentPtr->GetAttachParentActor();
		FloorPtr = Cast<AFloorHelper>(ParentPtr);
		if (FloorPtr)
		{
			break;
		}
	}

	if (FloorPtr)
	{
		BelongFloor = FloorPtr;
	}

	GetWorldTimerManager().SetTimer(QueryDeviceTimerHandel, this, &ThisClass::QueryDeviceInfo, IntervalTime, true);
}

TMap<FString, FString> ASceneElement_DeviceBase::GetStateDescription() const
{
	TMap<FString, FString> Result;

	if (bIsOpened)
	{
		Result.Add(TEXT(""), TEXT("开启"));
	}
	else
	{
		Result.Add(TEXT(""), TEXT("关闭"));
	}

	return Result;
}

void ASceneElement_DeviceBase::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	// {
	// 	auto EmptyContainer = FGameplayTagContainer::EmptyContainer;
	//
	// 	EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());
	//
	// 	if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
	// 	    EmptyContainer.Num())
	// 	{
	// 		SetActorHiddenInGame(true);
	//
	// 		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	// 		if (PrimitiveComponentPtr)
	// 		{
	// 			PrimitiveComponentPtr->SetRenderCustomDepth(false);
	// 		}
	//
	// 		return;
	// 	}
	// }
	// {
	// 	auto EmptyContainer = FGameplayTagContainer::EmptyContainer;
	//
	// 	EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
	// 	EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar.GetTag());
	//
	// 	if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
	// 	    EmptyContainer.Num())
	// 	{
	// 		SetActorHiddenInGame(false);
	//
	// 		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	// 		if (PrimitiveComponentPtr)
	// 		{
	// 			PrimitiveComponentPtr->SetRenderCustomDepth(true);
	// 			PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	// 		}
	//
	// 		return;
	// 	}
	// }
	// {
	// 	auto EmptyContainer = FGameplayTagContainer::EmptyContainer;
	//
	// 	EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
	//
	// 	if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
	// 	    EmptyContainer.Num())
	// 	{
	// 		SetActorHiddenInGame(false);
	//
	// 		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	// 		if (PrimitiveComponentPtr)
	// 		{
	// 			PrimitiveComponentPtr->SetRenderCustomDepth(false);
	// 		}
	//
	// 		return;
	// 	}
	// }
	// {
	// 	auto EmptyContainer = FGameplayTagContainer::EmptyContainer;
	//
	// 	EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_Focus.GetTag());
	//
	// 	if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
	// 		EmptyContainer.Num())
	// 	{
	// 		SetActorHiddenInGame(false);
	//
	// 		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
	// 		if (PrimitiveComponentPtr)
	// 		{
	// 			PrimitiveComponentPtr->SetRenderCustomDepth(true);
	// 			PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	// 		}
	//
	// 		return;
	// 	}
	// }
	// {
	// 	if (ConditionalSet.ConditionalSet.IsEmpty())
	// 	{
	// 	}
	// 	SetActorHiddenInGame(true);
	//
	// 	return;
	// }
}

void ASceneElement_DeviceBase::EntryFocusDevice()
{
}

void ASceneElement_DeviceBase::QuitFocusDevice()
{
}

void ASceneElement_DeviceBase::EntryViewDevice()
{
}

void ASceneElement_DeviceBase::QuitViewDevice()
{
}

void ASceneElement_DeviceBase::EntryShowDevice()
{
}

void ASceneElement_DeviceBase::QuitShowDevice()
{
}

void ASceneElement_DeviceBase::EntryShoweviceEffect()
{
}

void ASceneElement_DeviceBase::QuitShowDeviceEffect()
{
}

void ASceneElement_DeviceBase::QuitAllState()
{
	Super::QuitAllState();
}

TSharedPtr<FJsonValue> ASceneElement_DeviceBase::GetSceneElementData() const
{
	auto Result = Super::GetSceneElementData();

	auto RootJsonObj = Result->AsObject();

	RootJsonObj->SetStringField(
	                            TEXT("DeviceType"),
	                            DeviceType.ToString()
	                           );

	RootJsonObj->SetStringField(
	                            TEXT("DeviceTypeStr"),
	                            DeviceTypeStr
	                           );

	return Result;
}

void ASceneElement_DeviceBase::QueryDeviceInfo()
{
	if (Request.IsValid())
	{
		Request->CancelRequest();
	}

	Request = UWebChannelWorldSystem::GetInstance()->QueryDeviceID(
	                                                               SceneElementID,
	                                                               std::bind(
	                                                                         &ThisClass::QueryDeviceInfoComplete,
	                                                                         this,
	                                                                         std::placeholders::_1,
	                                                                         std::placeholders::_2
	                                                                        )
	                                                              );
}

void ASceneElement_DeviceBase::QueryDeviceInfoComplete(
	bool bSuccess,
	const FString& ResponStr
	)
{
	Request = nullptr;
	
	if (bSuccess)
	{
		FQueryDeviceInfo QueryDeviceInfo;
		QueryDeviceInfo.Deserialize(ResponStr);

		DeviceRealID = QueryDeviceInfo.ID;

		UpdateReletiveTransform(
		                        QueryDeviceInfo.Reletivetransform
		                       );
	}
}

void ASceneElement_DeviceBase::FQueryDeviceInfo::Deserialize(
	const FString& JsonStr
	)
{
	TSharedRef<TJsonReader<>> JsonReader =
		TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(
	                             JsonReader,
	                             jsonObject
	                            );

	if (jsonObject)
	{
	}
	else
	{
		return;
	}

	const TSharedPtr<FJsonObject>* body_OutObject = nullptr;
	if (jsonObject->TryGetObjectField(
	                                  TEXT("body"),
	                                  body_OutObject
	                                 ))
	{
		const TArray<TSharedPtr<FJsonValue>>* bimModelDeviceAssociations_OutArray = nullptr;
		if ((*body_OutObject)->TryGetArrayField(
		                                        TEXT("bimModelDeviceAssociations"),
		                                        bimModelDeviceAssociations_OutArray
		                                       ))
		{
			for (auto Iter : *bimModelDeviceAssociations_OutArray)
			{
				auto ObjSPtr = Iter->AsObject();
				if (ObjSPtr->TryGetStringField(
				                               TEXT("deviceId"),
				                               ID
				                              ))
				{
				}

#if UE_GAME
				if (ID == TEXT("0001-0406-2527-0289"))
				{
					checkNoEntry();
				}
#endif

				FString extra_Str;
				if (ObjSPtr->TryGetStringField(
				                               TEXT("extra"),
				                               extra_Str
				                              ))
				{
					TSharedRef<TJsonReader<>> extraJsonReader =
						TJsonReaderFactory<>::Create(extra_Str);

					TSharedPtr<FJsonObject> extra_OutObject;

					FJsonSerializer::Deserialize(
					                             extraJsonReader,
					                             extra_OutObject
					                            );

					const TSharedPtr<FJsonObject>* radarTransform_OutObject = nullptr;
					if (extra_OutObject->TryGetObjectField(
					                                       TEXT("radarTransform"),
					                                       radarTransform_OutObject
					                                      ))
					{
						FVector Translation;
						{
							const TSharedPtr<FJsonObject>* OutObject = nullptr;
							if ((*radarTransform_OutObject)->TryGetObjectField(
							                                                   TEXT("position"),
							                                                   OutObject
							                                                  ))
							{
								(*OutObject)->TryGetNumberField(
								                                TEXT("x"),
								                                Translation.X
								                               );
								(*OutObject)->TryGetNumberField(
								                                TEXT("y"),
								                                Translation.Y
								                               );
								(*OutObject)->TryGetNumberField(
								                                TEXT("z"),
								                                Translation.Z
								                               );
							}
						}
						FRotator Rotator;
						{
							const TSharedPtr<FJsonObject>* OutObject = nullptr;
							if ((*radarTransform_OutObject)->TryGetObjectField(
							                                                   TEXT("rotation"),
							                                                   OutObject
							                                                  ))
							{
								(*OutObject)->TryGetNumberField(
								                                TEXT("pitch"),
								                                Rotator.Pitch
								                               );
								(*OutObject)->TryGetNumberField(
								                                TEXT("yaw"),
								                                Rotator.Yaw
								                               );
								(*OutObject)->TryGetNumberField(
								                                TEXT("roll"),
								                                Rotator.Roll
								                               );
							}
						}
						FVector Scale;
						{
							const TSharedPtr<FJsonObject>* OutObject = nullptr;
							if ((*radarTransform_OutObject)->TryGetObjectField(
							                                                   TEXT("scale"),
							                                                   OutObject
							                                                  ))
							{
								(*OutObject)->TryGetNumberField(
								                                TEXT("x"),
								                                Scale.X
								                               );
								(*OutObject)->TryGetNumberField(
								                                TEXT("y"),
								                                Scale.Y
								                               );
								(*OutObject)->TryGetNumberField(
								                                TEXT("z"),
								                                Scale.Z
								                               );
							}
						}

						Reletivetransform = FTransform(Rotator, Translation, Scale);
					}
				}
				return;
			}
		}
	}
}

void ASceneElement_DeviceBase::UpdateCollisionBox(
	const TArray<UStaticMeshComponent*>& SMCompsAry
	)
{
	FBox Box(ForceInit);
	for (auto Iter : SMCompsAry)
	{
		Iter->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FBox TemoBox(ForceInit);
		TemoBox.IsValid = true;
		Iter->GetLocalBounds(TemoBox.Min, TemoBox.Max);
		TemoBox = TemoBox.TransformBy(Iter->GetRelativeTransform());

		Box += TemoBox;
	}

	CollisionComponentHelper->SetRelativeLocation(Box.GetCenter());

	CollisionComponentHelper->SetBoxExtent(Box.GetExtent());
}
