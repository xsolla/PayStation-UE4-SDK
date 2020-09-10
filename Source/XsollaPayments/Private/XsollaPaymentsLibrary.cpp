// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsLibrary.h"

#include "HttpModule.h"
#include "XsollaPaymentsDefines.h"

#include "Runtime/Launch/Resources/Version.h"
#include "XsollaPayments.h"
#include "XsollaPlayFabModels.h"

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
	case EXsollaPaymentsIntegration::Simplified:
		integrationTypeStr = TEXT("simplified");
		break;

	case EXsollaPaymentsIntegration::PlayFab:
		integrationTypeStr = TEXT("playfab");
		break;

	default:
		integrationTypeStr = TEXT("simplified");
	}

	FString sdkTag = FString::Printf(TEXT("SDK-payments_ver-%s_integr-%s_engine-%s_enginever-%s"), XSOLLA_PAYMENTS_VERSION, *integrationTypeStr, TEXT("UE4"), ENGINE_VERSION_STRING);
	return sdkTag;
}

TSharedRef<IHttpRequest> UXsollaPaymentsLibrary::CreateHttpRequest(const FString& Url,
	const EXsollaLoginRequestVerb Verb, const FString& Content, const FString& AuthToken)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(FString::Printf(TEXT("https://%s.playfabapi.com%s"), *FXsollaPaymentsModule::Get().GetTitleId(), *Url));

	switch (Verb)
	{
	case EXsollaLoginRequestVerb::GET:
		HttpRequest->SetVerb(TEXT("GET"));

		// Check that we doesn't provide content with GET request
		if (!Content.IsEmpty())
		{
			UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Request content is not empty for GET request. Maybe you should use POST one?"), *VA_FUNC_LINE);
		}
		break;

	case EXsollaLoginRequestVerb::POST:
		HttpRequest->SetVerb(TEXT("POST"));
		break;

	case EXsollaLoginRequestVerb::PUT:
		HttpRequest->SetVerb(TEXT("PUT"));
		break;

	case EXsollaLoginRequestVerb::DELETE:
		HttpRequest->SetVerb(TEXT("DELETE"));
		break;

	case EXsollaLoginRequestVerb::PATCH:
		HttpRequest->SetVerb(TEXT("PATCH"));
		break;

	default:
		unimplemented();
	}

	if (!Content.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		HttpRequest->SetContentAsString(Content);
	}

	if (!AuthToken.IsEmpty())
	{
		HttpRequest->SetHeader(TEXT("X-Authorization"), FString::Printf(TEXT("%s"), *AuthToken));
	}

	return HttpRequest;
}