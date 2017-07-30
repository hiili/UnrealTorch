// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class UnrealTorch : ModuleRules
{
	public UnrealTorch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Including Sol2 without this gives the error C4577 from <utility>
		// Alternative solution: disable 4577 in WindowsPlatformCompilerSetup.h
		//   (disabling 4577 in your own code is not enough, for some reason)
		// Also, we now actually use exceptions as they now work with the latest versions of everything.
		bEnableExceptions = true;

		PublicIncludePaths.AddRange(
			new string[] {
				"UnrealTorch/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"UnrealTorch/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects",
				"RangeV3",
				"Sol2",
				"Torch",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


		// Have our Lua files included during packaging
		//
		// cd Source/UnrealTorch/
		// find Private/lua/ -type f | sed 's_\(.*\)_RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ModuleDirectory, "\1")));_'
		//RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ModuleDirectory, "Private/lua/startup_module.lua")));
		//RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ModuleDirectory, "Private/lua/uth/tests.lua")));
		//RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ModuleDirectory, "Private/lua/uth/utility.lua")));
	}
}
