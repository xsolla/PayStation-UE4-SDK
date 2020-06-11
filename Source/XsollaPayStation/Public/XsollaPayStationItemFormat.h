// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"

#include "XsollaPayStationItemFormat.generated.h"

USTRUCT(Blueprintable)
struct XSOLLAPAYSTATION_API FXsollaItemPriceFormat
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString currency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	float amount;

	FXsollaItemPriceFormat()
		: amount(0.0f) {};
};

USTRUCT(Blueprintable)
struct XSOLLAPAYSTATION_API FXsollaVirtualCurrencyPackageFormat
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString currency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	float quantity;

	FXsollaVirtualCurrencyPackageFormat()
		: quantity(0.0f) {};
};

USTRUCT(Blueprintable)
struct XSOLLAPAYSTATION_API FXsollaItemFormat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString sku;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString display_name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString image_url;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FXsollaItemPriceFormat price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FXsollaVirtualCurrencyPackageFormat bundleContent;
};