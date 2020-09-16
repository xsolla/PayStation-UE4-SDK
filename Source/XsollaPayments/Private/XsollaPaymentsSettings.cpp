// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsSettings.h"

UXsollaPaymentsSettings::UXsollaPaymentsSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSandbox = true;
	bEnableSandboxInShipping = false;
	UsePlatformBrowser = false;
	PaymentInterfaceTheme = EXsollaPaymentsUiTheme::Dark;
	ProjectID = 68789;
}

FString UXsollaPaymentsSettings::GetTitleID() const
{
	return TitleID;
}
