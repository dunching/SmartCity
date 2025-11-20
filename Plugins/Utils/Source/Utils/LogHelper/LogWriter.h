// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include <string>
#include <mutex>

#include "CoreMinimal.h"

#include "LogWriter.generated.h"

#if UE_BUILD_SHIPPING 
#define WRITEINVOKELINE()
#else
#define WRITEINVOKELINE() 
#endif

#if UE_BUILD_SHIPPING 
#define PRINTFUNC()
#else
#define PRINTFUNC()\
{\
UE_LOG(LogTemp, Log, TEXT("%s %s %d"), *FString(__FILE__), *FString(__FUNCDNAME__), __LINE__); \
}
#endif

#if UE_BUILD_SHIPPING 
#define PRINTFUNCSTR(Str)
#else
#define PRINTFUNCSTR(Str)
#endif

#if UE_BUILD_SHIPPING 
#define WRITESCOPERUNDELTATIME()
#else
#define WRITESCOPERUNDELTATIME()
#endif

#if UE_BUILD_SHIPPING 
#define WRITEFUNCCALL()
#else
#define WRITEFUNCCALL()
#endif

#define PRINTINVOKEINFO() {\
UE_LOG(LogTemp, Log, TEXT("Func:%s Line:%d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__)\
}

#define PRINTINVOKEWITHSTR(Str) {\
UE_LOG(LogTemp, Log, TEXT("Func:%s Line:%d->%s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *Str)\
}

UCLASS()
class UTILS_API UKismetLogger : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 
	 * @param Catogory 
	 * @param LogLine 
	 * @param Day -1 则使用  >0：保留对应的天数
	 * @param Hour -1：保留最后 >0：保留对应的小时
	 */
	static void WriteLog(const FString& Catogory, const FString& LogLine, int32 Day, int32 Hour);
};
