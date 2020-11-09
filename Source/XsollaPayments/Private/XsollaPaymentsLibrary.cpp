// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsLibrary.h"
#include "XsollaPaymentsDefines.h"
#include "Runtime/Launch/Resources/Version.h"
#include "XsollaPayments.h"

UXsollaPaymentsSettings* UXsollaPaymentsLibrary::GetPaymentsSettings()
{
	return FXsollaPaymentsModule::Get().GetSettings();
}

FString UXsollaPaymentsLibrary::GeneratePurchaseUUID()
{
	return FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens).ToLower();
}

FString UXsollaPaymentsLibrary::Conv_XsollaPaymentsUiThemeToString(EXsollaPaymentsUiTheme theme)
{
	FString themeStr;

	switch (theme)
	{
	case EXsollaPaymentsUiTheme::Default:
		themeStr = TEXT("default");
		break;

	case EXsollaPaymentsUiTheme::DefaultDark:
		themeStr = TEXT("default_dark");
		break;

	case EXsollaPaymentsUiTheme::Dark:
		themeStr = TEXT("dark");
		break;

	default:
		themeStr = TEXT("dark");
	}

	return themeStr;
}

FString UXsollaPaymentsLibrary::GetPaymentsSdkTag(EXsollaPaymentsIntegration integrationType)
{
	FString integrationTypeStr;

	switch (integrationType)
	{
	case EXsollaPaymentsIntegration::Serverless:
		integrationTypeStr = TEXT("serverless");
		break;

	case EXsollaPaymentsIntegration::PlayFab:
		integrationTypeStr = TEXT("playfab");
		break;

	default:
		integrationTypeStr = TEXT("serverless");
	}

	FString sdkTag = FString::Printf(TEXT("SDK-payments_ver-%s_integr-%s_engine-%s_enginever-%s"), XSOLLA_PAYMENTS_VERSION, *integrationTypeStr, TEXT("UE4"), ENGINE_VERSION_STRING);
	return sdkTag;
}