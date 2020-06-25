// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaPayStationDataModel.generated.h"

USTRUCT(BlueprintType)
struct XSOLLAPAYSTATION_API FXsollaPayStationItemToPurchase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Xsolla PayStation Purchase")
	FString sku;

	UPROPERTY(BlueprintReadWrite, Category = "Xsolla PayStation Purchase")
	int32 amount;

	FXsollaPayStationItemToPurchase() 
		: amount(0) {};
};

USTRUCT(BlueprintType)
struct XSOLLAPAYSTATION_API FXsollaPayStationPurchaseStatus
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla PayStation Order")
	int32 id;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla PayStation Order")
	FString status;

	UPROPERTY(BlueprintReadOnly, Category = "Xsolla PayStation Order")
	FString external_id;

	FXsollaPayStationPurchaseStatus()
		: id(0) {};
};