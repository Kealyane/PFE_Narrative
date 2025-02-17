// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PFE_Narrative : ModuleRules
{
	public PFE_Narrative(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "EnhancedInput", 
            "Paper2D",
        });
	}
}
