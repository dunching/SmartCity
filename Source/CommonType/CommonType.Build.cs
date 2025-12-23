// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CommonType : ModuleRules
{
	public CommonType(ReadOnlyTargetRules Target) : base(Target)
    {
        bUseUnity = false;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Error;
        bWarningsAsErrors = true;
        bEnableExceptions = true;

        CppStandard = CppStandardVersion.Cpp20;
        
        bUseRTTI = true;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
        }

        PublicIncludePaths.Add("CommonType");
        PublicIncludePaths.Add("CommonType/CommonType");

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "DatasmithContent",
            
            "BuildingTools",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
        });

        // b*[^:b#/]+.*$
    }
}
