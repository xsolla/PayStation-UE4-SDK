// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationSettings.h"

UXsollaPayStationSettings::UXsollaPayStationSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSandbox = true;
	bEnableSandboxInShipping = false;
	UsePlatformBrowser = false;
}