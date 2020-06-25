// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationLibrary.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Engine/DataTable.h"

UXsollaPayStationSettings* UXsollaPayStationLibrary::GetPayStationSettings()
{
	return FXsollaPayStationModule::Get().GetSettings();
}

FString UXsollaPayStationLibrary::GeneratePurchaseUUID()
{
	return FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens).ToLower();
}

FString UXsollaPayStationLibrary::Conv_XsollaPayStationUiThemeToString(EXsollaPayStationUiTheme theme)
{
	FString themeStr;

	switch (theme)
	{
	case EXsollaPayStationUiTheme::Default:
		themeStr = TEXT("default");
		break;

	case EXsollaPayStationUiTheme::DefaultDark:
		themeStr = TEXT("default_dark");
		break;

	case EXsollaPayStationUiTheme::Dark:
		themeStr = TEXT("dark");
		break;

	default:
		themeStr = TEXT("dark");
	}

	return themeStr;
}


