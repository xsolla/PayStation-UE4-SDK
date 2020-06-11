// Copyright 2020 Xsolla Inc. All Rights Reserved.

using UnrealBuildTool;

public class XsollaPayStation : ModuleRules
{
	public XsollaPayStation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"HTTP",
				"Json",
				"JsonUtilities",
				"UMG",
				"XsollaPayStationBrowser"
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

		PublicDefinitions.Add("WITH_XSOLLA_PAYSTATION=1");
	}
}
