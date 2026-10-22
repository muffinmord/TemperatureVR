// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class temperature_grid : ModuleRules
{
	public temperature_grid(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.Add(
           @"F:\studium\grib2dec-master(1)\grib2dec-master\include"
        );
    }
}
