#include "MessageBody.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

#include "Tools.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "LogWriter.h"
#include "SceneInteractionDecorator.h"

FMessageBody::FMessageBody()
{
	Guid = FGuid::NewGuid();
}

FMessageBody::~FMessageBody()
{
}

TSharedPtr<FJsonObject> FMessageBody::Deserialize(const FString& JsonStr)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);

	TSharedPtr<FJsonObject> jsonObject;

	FJsonSerializer::Deserialize(JsonReader,
								 jsonObject);

	if (jsonObject->TryGetStringField(TEXT("Name"),Name))
	{
	}

	FString GuidStr;
	if (jsonObject->TryGetStringField(TEXT("Guid"),GuidStr))
	{
		Guid  = FGuid(JsonStr);
	}

	return jsonObject;
}

TSharedPtr<FJsonObject> FMessageBody::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = MakeShareable<FJsonObject>(new FJsonObject);
	
	RootJsonObj->SetStringField(TEXT("Name"),
	                            Name);
	
	RootJsonObj->SetStringField(TEXT("Guid"),
	                            Guid.ToString());

	return RootJsonObj;
}

FMessageBody_SelectedSpace::FMessageBody_SelectedSpace()
{
	Name = TEXT("SelectedSpace");
}

TSharedPtr<FJsonObject> FMessageBody_SelectedSpace::Deserialize(const FString& JsonStr)
{
	return FMessageBody::Deserialize(JsonStr);
}

TSharedPtr<FJsonObject> FMessageBody_SelectedSpace::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = FMessageBody::SerializeBody();

	RootJsonObj->SetStringField(TEXT("SpaceName"),
								SpaceName);
	
	return RootJsonObj;
}

FMessageBody_SelectedDevice::FMessageBody_SelectedDevice()
{
	Name = TEXT("SelectedDevice");
}

TSharedPtr<FJsonObject> FMessageBody_SelectedDevice::SerializeBody() const
{
	TSharedPtr<FJsonObject> RootJsonObj = FMessageBody::SerializeBody();

	RootJsonObj->SetStringField(TEXT("DeviceID"),
								DeviceID);
	
	return RootJsonObj;
}

FString FMessageBody::GetJsonString() const
{
	TSharedPtr<FJsonObject> RootJsonObj = SerializeBody();

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(RootJsonObj.ToSharedRef(),
	                           Writer);

	return JsonString;
}

FString FMessageBody::GetName() const
{
	return Name;
}
