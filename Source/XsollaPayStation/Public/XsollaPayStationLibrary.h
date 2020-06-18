// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaPayStationDataModel.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaPayStationLibrary.generated.h"

class UXsollaPayStationSettings;
class UDataTable;

UCLASS()
class XSOLLAPAYSTATION_API UXsollaPayStationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Direct access to PayStation SDK settings */
	UFUNCTION(BlueprintPure, Category = "Xsolla | PayStation")
	static UXsollaPayStationSettings* GetPayStationSettings();

	/** Generate unique purchase identifier */
	UFUNCTION(BlueprintPure, Category = "Xsolla | PayStation")
	static FString GeneratePurchaseUUID();
};