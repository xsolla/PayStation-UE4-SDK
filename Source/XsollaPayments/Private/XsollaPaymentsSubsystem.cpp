// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsSubsystem.h"



#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "XsollaPayments.h"
#include "XsollaPaymentsDefines.h"
#include "XsollaPaymentsSettings.h"
#include "XsollaPaymentsImageLoader.h"

#include "Engine/Engine.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "FXsollaPaymentsModule"

const FString UXsollaPaymentsSubsystem::PaymentEndpoint(TEXT("https://secure.xsolla.com/paystation3"));
const FString UXsollaPaymentsSubsystem::SandboxPaymentEndpoint(TEXT("https://sandbox-secure.xsolla.com/paystation3"));

UXsollaPaymentsSubsystem::UXsollaPaymentsSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(TEXT("/XsollaPayments/Browser/W_PaymentsBrowser.W_PaymentsBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
}

void UXsollaPaymentsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Check whether image loader exists, because initialization can be called multiple times
	if (!ImageLoader)
	{
		ImageLoader = NewObject<UXsollaPaymentsImageLoader>();
	}

	const UXsollaPaymentsSettings* Settings = FXsollaPaymentsModule::Get().GetSettings();

	ProjectID = Settings->ProjectID;

	UE_LOG(LogXsollaPayments, Log, TEXT("%s: XsollaPayments subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaPaymentsSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaPaymentsSubsystem::LaunchPaymentConsoleWithAccessToken(const FString& AccessToken, UUserWidget*& BrowserWidget)
{
	const FString Endpoint = IsSandboxEnabled() ? SandboxPaymentEndpoint : PaymentEndpoint;
	const FString PaymentsUrl = FString::Printf(TEXT("%s?access_token=%s"), *Endpoint, *AccessToken);

	OpenBrowser(PaymentsUrl, BrowserWidget);
}

void UXsollaPaymentsSubsystem::LaunchPaymentConsoleWithAccessData(FXsollaPaymentsAccessData accessData, UUserWidget*& BrowserWidget)
{
	// Prepare access data for payment
	FString accessDataStr;
	if (!FJsonObjectConverter::UStructToJsonObjectString(accessData, accessDataStr, 0, 0, 0, nullptr, false))
	{
		UE_LOG(LogXsollaPayments, Error, TEXT("%s: Failed to parse payment acces data"), *VA_FUNC_LINE);
	}
	
	// fix property names that were altered by FJsonObjectConverter::UStructToJsonObjectString function
	accessDataStr = accessDataStr.Replace(TEXT("uI"), TEXT("ui")).Replace(TEXT("iD"), TEXT("id"));

	const FString Endpoint = IsSandboxEnabled() ? SandboxPaymentEndpoint : PaymentEndpoint;
	const FString PaymentsUrl = FString::Printf(TEXT("%s?access_data=%s"), *Endpoint, *FGenericPlatformHttp::UrlEncode(accessDataStr));

	OpenBrowser(PaymentsUrl, BrowserWidget);
}

FString UXsollaPaymentsSubsystem::GetPendingPaymentsUrl() const
{
	return PengindPaymentsUrl;
}

void UXsollaPaymentsSubsystem::CheckPurchaseStatus(const FString& PurchaseUUID, const FOnCheckPurchaseStatusSuccess& SuccessCallback, const FOnPaymentsError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://api.xsolla.com/merchant/projects/%d/transactions/external/%s/status"), ProjectID, *PurchaseUUID);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaPaymentsSubsystem::CheckPurchaseStatus_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

bool UXsollaPaymentsSubsystem::IsSandboxEnabled() const
{
	const UXsollaPaymentsSettings* Settings = FXsollaPaymentsModule::Get().GetSettings();
	bool bIsSandboxEnabled = Settings->bSandbox;

#if UE_BUILD_SHIPPING
	bIsSandboxEnabled = Settings->bSandbox && Settings->bEnableSandboxInShipping;
	if (bIsSandboxEnabled)
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Sandbox should be disabled in Shipping build"), *VA_FUNC_LINE);
	}
#endif // UE_BUILD_SHIPPING

	return bIsSandboxEnabled;
}

void UXsollaPaymentsSubsystem::OpenBrowser(const FString& Url, UUserWidget*& BrowserWidget)
{
	UE_LOG(LogXsollaPayments, Log, TEXT("%s: Loading Payments: %s"), *VA_FUNC_LINE, *Url);

	const UXsollaPaymentsSettings* Settings = FXsollaPaymentsModule::Get().GetSettings();

	if (Settings->UsePlatformBrowser)
	{
		BrowserWidget = nullptr;

		FPlatformProcess::LaunchURL(*Url, nullptr, nullptr);
	}
	else
	{
		// Check for user browser widget override
		auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

		PengindPaymentsUrl = Url;
		auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
		MyBrowser->AddToViewport(MAX_int32);

		BrowserWidget = MyBrowser;
	}
}

void UXsollaPaymentsSubsystem::CheckPurchaseStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCheckPurchaseStatusSuccess SuccessCallback, FOnPaymentsError ErrorCallback)
{
	if (HandlePurchaseStatusError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaPayments, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), TEXT("Can't deserialize server response"));
		return;
	}

	FXsollaPaymentsPurchaseStatus purchaseStatus;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaPaymentsPurchaseStatus::StaticStruct(), &purchaseStatus))
	{
		UE_LOG(LogXsollaPayments, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), TEXT("Can't convert server response to struct"));
		return;
	}

	SuccessCallback.ExecuteIfBound(purchaseStatus);
}

bool UXsollaPaymentsSubsystem::HandlePurchaseStatusError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnPaymentsError ErrorCallback)
{
	int32 StatusCode = 0;
	FString ErrorStr;
	FString ResponseStr = TEXT("Invalid");

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();

		if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			StatusCode = HttpResponse->GetResponseCode();
			ErrorStr = FString::Printf(TEXT("Invalid response. Сode=%d, Error=%s"), HttpResponse->GetResponseCode(), *ResponseStr);

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				static const FString StatucCodeFieldName = TEXT("http_status_code");
				if (JsonObject->HasTypedField<EJson::Number>(StatucCodeFieldName))
				{
					StatusCode = JsonObject->GetNumberField(StatucCodeFieldName);
					ErrorStr = JsonObject->GetStringField(TEXT("message"));
				}
				else
				{
					ErrorStr = FString::Printf(TEXT("Can't deserialize error json: no field '%s' found"), *StatucCodeFieldName);
				}
			}
			else
			{
				ErrorStr = TEXT("Can't deserialize error json");
			}
		}
	}
	else
	{
		ErrorStr = TEXT("No response");
	}

	if (!ErrorStr.IsEmpty())
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: request failed (%s): %s"), *VA_FUNC_LINE, *ErrorStr, *ResponseStr);
		ErrorCallback.ExecuteIfBound(StatusCode, ErrorStr);
		return true;
	}

	return false;
}

TSharedRef<IHttpRequest> UXsollaPaymentsSubsystem::CreateHttpRequest(const FString& Url) const
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=Payments&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_PAYMENTS_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), TEXT("PAYMENTS"));
	HttpRequest->SetHeader(TEXT("X-SDK-V"), XSOLLA_PAYMENTS_VERSION);

	return HttpRequest;
}

void UXsollaPaymentsSubsystem::LoadImageFromWeb(const FString& URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback)
{
	ImageLoader->LoadImage(URL, SuccessCallback, ErrorCallback);
}

#undef LOCTEXT_NAMESPACE