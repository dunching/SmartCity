// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AssetRef : ModuleRules
{
	public AssetRef(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.Add("AssetRef");
        PublicIncludePaths.Add("AssetRef/AssetRef");

        PrivateDependencyModuleNames.AddRange(new string[] { });
        
        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayAbilities",

            // "Planet",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
        });

        // b*[^:b#/]+.*$
    }
}
