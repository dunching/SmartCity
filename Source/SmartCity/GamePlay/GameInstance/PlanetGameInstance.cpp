
#include "PlanetGameInstance.h"

#include "GameFramework/PlayerInput.h"

#include "LogHelper/LogWriter.h"

#include "InputProcessorSubSystemBase.h"
#include "IPSSI.h"

void UPlanetGameInstance::Init()
{
	Super::Init();
}

void UPlanetGameInstance::OnStart()
{
	Super::OnStart();

	// 之前的代码，为什么加这句记不起来了
	// GetWorldImp()->SetGameInstance(this);

	GIsExiting = false;
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
