// Copyright 2020 Xsolla Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class XsollaPaymentsBrowser : ModuleRules
    {
        public XsollaPaymentsBrowser(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "WebBrowser",
                    "Slate",
                    "SlateCore",
                    "UMG",
                    "Engine"
                }
            );
            
            if (Target.bBuildEditor || Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.IOS)
            {
                PrivateIncludePathModuleNames.AddRange(
                    new string[]
                    {
                        "WebBrowserTexture",
                    }
                );

                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "WebBrowserTexture",
                    }
                );
            }
        }
    }
}