// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationLibrary.h"
#include "XsollaPayStationSettings.h"

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

