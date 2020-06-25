// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaPayStationDataModel.h"
#include "XsollaPayStationSettings.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaPayStationLibrary.generated.h"

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

	/** Convert Payments UI theme to acceptable string format */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "PaymentsUiTheme To String", CompactNodeTitle = "->", BlueprintAutocast), Category = "Xsolla | PayStation")
	static FString Conv_XsollaPayStationUiThemeToString(EXsollaPayStationUiTheme theme);
};