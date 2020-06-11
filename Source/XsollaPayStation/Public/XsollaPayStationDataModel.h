// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaPayStationDataModel.generated.h"

USTRUCT(BlueprintType)
struct XSOLLAPAYSTATION_API FXsollaPayStationPrice
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FString currency;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	float amount;

	FXsollaPayStationPrice()
		: amount(0.0f) {};
};

USTRUCT(BlueprintType)
struct XSOLLAPAYSTATION_API FXsollaPayStationVirtualCurrencyPackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FString currency;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	float quantity;

	FXsollaPayStationVirtualCurrencyPackage()
		: quantity(0.0f) {};
};

USTRUCT(BlueprintType)
struct XSOLLAPAYSTATION_API FXsollaPayStationItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FString sku;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FString type;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FString display_name;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FString description;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FString image_url;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FXsollaPayStationPrice price;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla Item")
	FXsollaPayStationVirtualCurrencyPackage bundleContent;
};