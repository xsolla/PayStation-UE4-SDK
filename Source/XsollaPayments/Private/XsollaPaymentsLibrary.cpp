// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsLibrary.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Engine/DataTable.h"

UXsollaPaymentsSettings* UXsollaPaymentsLibrary::GetPaymentsSettings()
{
	return FXsollaPaymentsModule::Get().GetSettings();
}

FString UXsollaPaymentsLibrary::GeneratePurchaseUUID()
{
	return FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens).ToLower();
}

FString UXsollaPaymentsLibrary::Conv_XsollaPaymentsUiThemeToString(EXsollaPaymentsUiTheme theme)
{
	FString themeStr;

	switch (theme)
	{
	case EXsollaPaymentsUiTheme::Default:
		themeStr = TEXT("default");
		break;

	case EXsollaPaymentsUiTheme::DefaultDark:
		themeStr = TEXT("default_dark");
		break;

	case EXsollaPaymentsUiTheme::Dark:
		themeStr = TEXT("dark");
		break;

	default:
		themeStr = TEXT("dark");
	}

	return themeStr;
}


