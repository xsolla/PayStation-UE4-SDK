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

void UXsollaPayStationLibrary::ParseStoreItemsDataTable(UDataTable* ItemsDataTable, TArray<FXsollaPayStationItem>& StoreItems)
{
	FString itemsDataJson = ItemsDataTable->GetTableAsJSON();

	UE_LOG(LogXsollaPayStation, Verbose, TEXT("%s: Preparing to parse store items data JSON: %s"), *VA_FUNC_LINE, *itemsDataJson);

	TArray<FXsollaPayStationItem> storeItemsData;
	if (FJsonObjectConverter::JsonArrayStringToUStruct(itemsDataJson, &storeItemsData, 0, 0))
	{
		StoreItems = storeItemsData;
		return;
	}

	UE_LOG(LogXsollaPayStation, Error, TEXT("%s: Can't parse items data"), *VA_FUNC_LINE);
}
