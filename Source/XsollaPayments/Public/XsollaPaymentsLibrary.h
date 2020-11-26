// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "XsollaPaymentsSettings.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaPaymentsImageLoader.h"
#include "XsollaPaymentsLibrary.generated.h"

/** Type of Payments SDK integration */
UENUM(BlueprintType)
enum class EXsollaPaymentsIntegration : uint8
{
    Serverless,
    PlayFab
};

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
    UFUNCTION(BlueprintPure, meta = (DisplayName = "PaymentsUiTheme To String", CompactNodeTitle = "->",
        BlueprintAutocast), Category = "Xsolla | Payments")
    static FString Conv_XsollaPaymentsUiThemeToString(EXsollaPaymentsUiTheme theme);

    /** Get extra data for Payments SDK analytics */
    UFUNCTION(BlueprintPure, Category = "Xsolla | Payments")
    static FString GetPaymentsSdkTag(EXsollaPaymentsIntegration integrationType);
};
