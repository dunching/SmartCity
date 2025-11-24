#include "LogWriter.h"

void UKismetLogger::WriteLog(
	const FString& Catogory,
	const FString& LogLine,
	int32 Day,
	int32 Hour
	)
{
	FDateTime NowTime = FDateTime::Now();

	const auto FilePath = FString::Printf(TEXT("%sSmartCityLogs/%s/"), *FPaths::ProjectContentDir(), *Catogory);

	const FString FullPath = FPaths::ConvertRelativePathToFull(FilePath); // 转换为绝对路径

	TArray<FString> NeedDeleteDirAry;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// PlatformFile.SetSandboxEnabled(false);
	
	PlatformFile.IterateDirectory(
	                              *FullPath,
	                              [&NowTime, &Day, &Hour,&NeedDeleteDirAry](
	                              const TCHAR* Name,
	                              bool bIsFolder
	                              )
	                              {
		                              FDateTime Time;
		                              if (bIsFolder)
		                              {
			                              const auto FileTime = FPaths::GetCleanFilename(Name);
			                              if (FDateTime::Parse(FileTime + TEXT(".0.0"), Time))
			                              {
				                              auto Offset = NowTime - Time;
				                              if (Day > 0)
				                              {
					                              const auto CurrentDay = Offset.GetDays();
					                              if (CurrentDay > Day)
					                              {
						                              NeedDeleteDirAry.Add(Name);
					                              }
				                              }
				                              else if (Hour > 0)
				                              {
					                              const auto CurrentHour = Offset.GetHours();
					                              if (CurrentHour > Hour)
					                              {
						                              NeedDeleteDirAry.Add(Name);
					                              }
				                              }
			                              }
		                              }

		                              return true;
	                              }
	                             );

	for (const auto& Iter : NeedDeleteDirAry)
	{
		if (PlatformFile.DirectoryExists(*Iter))
		{
			if (PlatformFile.DeleteDirectoryRecursively(*Iter))
			{}
		}
	}
	
	const auto NowTimeDay = NowTime.ToString(TEXT("%Y.%m.%d-%H"));

	const auto NowTimeHour = NowTime.ToString(TEXT("%M.%S"));

	const auto NewFile = FString::Printf(TEXT("%s/%s/%s.txt"), *FilePath, *NowTimeDay, *NowTimeHour);

	TArray<FString> LogLines;
	if (FFileHelper::LoadFileToStringArray(LogLines, *NewFile))
	{
	}
	LogLines.Add(LogLine);

	FFileHelper::SaveStringArrayToFile(LogLines, *NewFile);

}
