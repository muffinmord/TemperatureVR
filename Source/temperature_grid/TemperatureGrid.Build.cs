// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class temperature_grid : ModuleRules
{
	public temperature_grid(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PrivateIncludePaths.Add(
            Path.Combine(
                ModuleDirectory,
                "../../ThirdParty/grib2dec-master/include"
            )
        );
	}
}
