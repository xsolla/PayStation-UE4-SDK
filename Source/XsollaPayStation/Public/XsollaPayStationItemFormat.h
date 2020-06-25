// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"

#include "XsollaPayStationItemFormat.generated.h"

USTRUCT(Blueprintable)
struct XSOLLAPAYSTATION_API FXsollaItemPrice
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString currency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	float amount;

	FXsollaItemPrice()
		: amount(0.0f) {};
};

USTRUCT(Blueprintable)
struct XSOLLAPAYSTATION_API FXsollaVirtualCurrencyPackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FString currency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	float quantity;

	FXsollaVirtualCurrencyPackage()
		: quantity(0.0f) {};
};

USTRUCT(Blueprintable)
struct XSOLLAPAYSTATION_API FXsollaCatalogItem : public FTableRowBase
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
	FXsollaItemPrice price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla Formats")
	FXsollaVirtualCurrencyPackage bundle_content;
};