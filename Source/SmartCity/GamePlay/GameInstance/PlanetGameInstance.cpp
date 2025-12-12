#include "PlanetGameInstance.h"

#include <Windows.Data.Text.h>
#include "GameFramework/PlayerInput.h"
#include "Windows/WindowsApplication.h"

#include "LogHelper/LogWriter.h"

#include "InputProcessorSubSystemBase.h"
#include "IPSSI.h"

void UPlanetGameInstance::Init()
{
	Super::Init();
}

void UPlanetGameInstance::SetTaskbarWindowName(
	const FString& NewWindowName
	)
{
	// 仅在Windows平台生效
#if PLATFORM_WINDOWS

	// FApp::SetProjectName(*NewWindowName);

	UKismetSystemLibrary::SetWindowTitle(FText::FromString(NewWindowName));

#endif
}

void UPlanetGameInstance::OnStart()
{
	Super::OnStart();

	// 之前的代码，为什么加这句记不起来了
	// GetWorldImp()->SetGameInstance(this);

	GIsExiting = false;

	// 1. 获取原始命令行
	FString CmdLine = FCommandLine::Get();
	UE_LOG(LogTemp, Log, TEXT("原始命令行：%s"), *CmdLine);

	// 2. 解析无值参数（如 -TestMode）
	TArray<FString> Tokens;
	TArray<FString> Switches;
	FCommandLine::Parse(*CmdLine, Tokens, Switches);

	// 3. 解析带值参数（如 -CustomParam=MyValue）
	FString CustomTitle;
	if (FParse::Value(*CmdLine, TEXT("PixelStreamingPort="), CustomTitle))
	{
		// SetTaskbarWindowName(FString::Printf( TEXT("SmartCity:%s"), *CustomTitle));
	}
}

void UPlanetGameInstance::Shutdown()
{
	GIsExiting = true;

	// 注意：这里提前释放的会，之后编辑器模式下保存资源会出错
	// GUObjectArray.ShutdownUObjectArray();

#if WITH_EDITOR
#endif

	UInputProcessorSubSystem_Imp::GetInstance()->ResetProcessor();

	Super::Shutdown();
	PRINTFUNC();
}

void UPlanetGameInstance::FinishDestroy()
{
	Super::FinishDestroy();
	PRINTFUNC();
}

UPlanetGameInstance::~UPlanetGameInstance()
{
	PRINTFUNC();
}
