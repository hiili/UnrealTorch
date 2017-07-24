// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class RangeV3 : ModuleRules
{
	public RangeV3(TargetInfo Target)
	{
		Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Range-V3-VS2015/include"));
	}
}
