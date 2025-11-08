// Copyright 2019 Tefel. All Rights Reserved

using System.IO;
using UnrealBuildTool;

public class BuildingTools : ModuleRules
{
	public BuildingTools(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        CppStandard = CppStandardVersion.Cpp20;

        PublicIncludePaths.AddRange(
			new string[] {
                   Path.Combine(PluginDirectory, "Source/BuildingTools"),
                   Path.Combine(PluginDirectory, "Source/CommonType"),
                   Path.Combine(PluginDirectory, "Source/SceneElement"),
            }
			);
				
		PrivateIncludePaths.AddRange(
			new string[] {
            }
			);

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
            }
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "GameplayTags",
                "Json",
                "DatasmithContent",

                "Effects",
                "SmartCitySuite", "Utils", 
            }
			);

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
            {
            }
			);
	}
}
