// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaPaymentsDataModel.h"
#include "XsollaPaymentsSettings.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "XsollaPaymentsLibrary.generated.h"

class UDataTable;

UCLASS()
class XSOLLAPAYMENTS_API UXsollaPaymentsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Direct access to Payments SDK settings */
	UFUNCTION(BlueprintPure, Category = "Xsolla | Payments")
	static UXsollaPaymentsSettings* GetPaymentsSettings();

	/** Generate unique purchase identifier */
	UFUNCTION(BlueprintPure, Category = "Xsolla | Payments")
	static FString GeneratePurchaseUUID();

	/** Convert Payments UI theme to acceptable string format */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "PaymentsUiTheme To String", CompactNodeTitle = "->", BlueprintAutocast), Category = "Xsolla | Payments")
	static FString Conv_XsollaPaymentsUiThemeToString(EXsollaPaymentsUiTheme theme);
};