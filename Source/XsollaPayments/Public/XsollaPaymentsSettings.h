// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "XsollaPaymentsSettings.generated.h"

/** User interface theme for payment interface*/
UENUM(BlueprintType)
enum class EXsollaPaymentsUiTheme : uint8
{
	Default,
	DefaultDark,
	Dark
};

UCLASS(config = Engine, defaultconfig)
class XSOLLAPAYMENTS_API UXsollaPaymentsSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/** Flag indicating whether sandbox mode for payments is enabled */
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Xsolla Payments Settings")
	bool bSandbox;

	/** Flag indicating whether sandbox mode can be used in Shipping build */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Payments Settings")
	bool bEnableSandboxInShipping;

	/** Custom class to present payment console user interface */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Payments Settings")
	TSubclassOf<UUserWidget> OverrideBrowserWidgetClass;

	/** Enable to process a payment with an external (system) browser. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Payments Settings")
	bool UsePlatformBrowser;

	/** Payment user interface theme. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Payments Settings")
	EXsollaPaymentsUiTheme PaymentInterfaceTheme;

	/** Project ID from your Publisher Account. Required if simplified integration flow is preferred. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Payments Settings | Simplified integration")
	int32 ProjectID;

	/** Title ID from your PlayFab Developer Account. Required if PlayFab integration flow is prefered */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Xsolla Payments Settings | PlayFab")
	FString TitleID;
};