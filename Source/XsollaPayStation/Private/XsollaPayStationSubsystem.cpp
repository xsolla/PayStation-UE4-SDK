// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationSubsystem.h"

#include "XsollaPayStation.h"
#include "XsollaPayStationDefines.h"
#include "XsollaPayStationSettings.h"
#include "XsollaPayStationImageLoader.h"

#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "FXsollaPayStationModule"

const FString UXsollaPayStationSubsystem::PaymentEndpoint(TEXT("https://secure.xsolla.com/paystation3"));
const FString UXsollaPayStationSubsystem::SandboxPaymentEndpoint(TEXT("https://sandbox-secure.xsolla.com/paystation3"));

UXsollaPayStationSubsystem::UXsollaPayStationSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(TEXT("/XsollaPayStation/Browser/W_PayStationBrowser.W_PayStationBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
}

void UXsollaPayStationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Check whether image loader exists, because initialization can be called multiple times
	if (!ImageLoader)
	{
		ImageLoader = NewObject<UXsollaPayStationImageLoader>();
	}

	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();

	ProjectID = Settings->ProjectID;

	UE_LOG(LogXsollaPayStation, Log, TEXT("%s: XsollaPayStation subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaPayStationSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaPayStationSubsystem::LaunchPaymentConsole(const FString& PaymentToken, UUserWidget*& BrowserWidget)
{
	const FString Endpoint = IsSandboxEnabled() ? SandboxPaymentEndpoint : PaymentEndpoint;
	const FString PayStationUrl = FString::Printf(TEXT("%s?access_token=%s"), *Endpoint, *PaymentToken);

	UE_LOG(LogXsollaPayStation, Log, TEXT("%s: Loading PayStation: %s"), *VA_FUNC_LINE, *PayStationUrl);

	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();

	if (Settings->UsePlatformBrowser)
	{
		BrowserWidget = nullptr;

		FPlatformProcess::LaunchURL(*PayStationUrl, nullptr, nullptr);
	}
	else
	{
		// Check for user browser widget override
		auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

		PengindPayStationUrl = PayStationUrl;
		auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
		MyBrowser->AddToViewport(MAX_int32);

		BrowserWidget = MyBrowser;
	}
}

void UXsollaPayStationSubsystem::LaunchPaymentConsoleWithAccessData(const TArray<FXsollaPayStationItemToPurchase>& ItemsToPurchase, const FString& PurchaseUUID, UUserWidget*& BrowserWidget)
{
	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();

	// Prepare access data for PayStation
	TSharedPtr<FJsonObject> AccessDataJson = MakeShareable(new FJsonObject);

	// Fill user data
	TSharedPtr<FJsonObject> UserIdJson = MakeShareable(new FJsonObject);
	UserIdJson->SetStringField(TEXT("value"), Settings->ProjectID);

	TSharedPtr<FJsonObject> UserJson = MakeShareable(new FJsonObject);
	UserJson->SetObjectField(TEXT("id"), UserIdJson);

	AccessDataJson->SetObjectField(TEXT("user"), UserJson);

	// Fill PayStation settings
	TSharedPtr<FJsonObject> PaymentUiSettingsJson = MakeShareable(new FJsonObject);
	PaymentUiSettingsJson->SetStringField(TEXT("theme"), TEXT("default_dark"));

	TSharedPtr<FJsonObject> PaymentSettingsJson = MakeShareable(new FJsonObject);
	PaymentSettingsJson->SetObjectField(TEXT("ui"), PaymentUiSettingsJson);
	PaymentSettingsJson->SetStringField(TEXT("external_id"), PurchaseUUID);
	PaymentSettingsJson->SetNumberField(TEXT("project_id"), FCString::Atoi(*ProjectID));

	if (IsSandboxEnabled())
	{
		PaymentSettingsJson->SetStringField(TEXT("mode"), TEXT("sandbox"));
	}

	AccessDataJson->SetObjectField(TEXT("settings"), PaymentSettingsJson);

	// Fill purchase data
	TArray<TSharedPtr<FJsonValue>> VirtualItemsJsonArray;
	for (auto Item : ItemsToPurchase)
	{
		TSharedRef<FJsonObject> ItemJson = MakeShareable(new FJsonObject());
		if (FJsonObjectConverter::UStructToJsonObject(FXsollaPayStationItemToPurchase::StaticStruct(), &Item, ItemJson, 0, 0))
		{
			VirtualItemsJsonArray.Push(MakeShareable(new FJsonValueObject(ItemJson)));
		}
	}

	TSharedPtr<FJsonObject> VirtualItemsJson = MakeShareable(new FJsonObject);
	VirtualItemsJson->SetArrayField(TEXT("items"), VirtualItemsJsonArray);

	TSharedPtr<FJsonObject> PurchaseJson = MakeShareable(new FJsonObject);
	PurchaseJson->SetObjectField(TEXT("virtual_items"), VirtualItemsJson);

	AccessDataJson->SetObjectField(TEXT("purchase"), PurchaseJson);

	// Convert access data to string
	FString accessDataStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&accessDataStr);
	FJsonSerializer::Serialize(AccessDataJson.ToSharedRef(), Writer);

	// Minify access data json
	accessDataStr = accessDataStr.Replace(TEXT("\n"), TEXT("")).Replace(TEXT(" "), TEXT(""));

	// Launch PayStation
	const FString Endpoint = IsSandboxEnabled() ? SandboxPaymentEndpoint : PaymentEndpoint;
	const FString PayStationUrl = FString::Printf(TEXT("%s?access_data=%s"), *Endpoint, *FGenericPlatformHttp::UrlEncode(accessDataStr));

	UE_LOG(LogXsollaPayStation, Log, TEXT("%s: Loading PayStation: %s"), *VA_FUNC_LINE, *PayStationUrl);

	if (Settings->UsePlatformBrowser)
	{
		BrowserWidget = nullptr;

		FPlatformProcess::LaunchURL(*PayStationUrl, nullptr, nullptr);
	}
	else
	{
		// Check for user browser widget override
		auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

		PengindPayStationUrl = PayStationUrl;
		auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
		MyBrowser->AddToViewport(MAX_int32);

		BrowserWidget = MyBrowser;
	}
}

FString UXsollaPayStationSubsystem::GetPendingPayStationUrl() const
{
	return PengindPayStationUrl;
}

void UXsollaPayStationSubsystem::CheckPurchaseStatus(const FString& PurchaseUUID, const FOnCheckPurchaseStatusSuccess& SuccessCallback, const FOnPayStationError& ErrorCallback)
{
	const FString Url = FString::Printf(TEXT("https://api.xsolla.com/merchant/projects/%s/transactions/external/%s/status"), *ProjectID, *PurchaseUUID);

	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaPayStationSubsystem::CheckPurchaseStatus_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

bool UXsollaPayStationSubsystem::IsSandboxEnabled() const
{
	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();
	bool bIsSandboxEnabled = Settings->bSandbox;

#if UE_BUILD_SHIPPING
	bIsSandboxEnabled = Settings->bSandbox && Settings->bEnableSandboxInShipping;
	if (bIsSandboxEnabled)
	{
		UE_LOG(LogXsollaPayStation, Warning, TEXT("%s: Sandbox should be disabled in Shipping build"), *VA_FUNC_LINE);
}
#endif // UE_BUILD_SHIPPING

	return bIsSandboxEnabled;
}

void UXsollaPayStationSubsystem::CheckPurchaseStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCheckPurchaseStatusSuccess SuccessCallback, FOnPayStationError ErrorCallback)
{
	if (HandlePurchaseStatusError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayStation, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *ResponseStr);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogXsollaPayStation, Error, TEXT("%s: Can't deserialize server response"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), TEXT("Can't deserialize server response"));
		return;
	}

	FXsollaPayStationPurchaseStatus purchaseStatus;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaPayStationPurchaseStatus::StaticStruct(), &purchaseStatus))
	{
		UE_LOG(LogXsollaPayStation, Error, TEXT("%s: Can't convert server response to struct"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(HttpResponse->GetResponseCode(), TEXT("Can't convert server response to struct"));
		return;
	}

	SuccessCallback.ExecuteIfBound(purchaseStatus);
}

bool UXsollaPayStationSubsystem::HandlePurchaseStatusError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnPayStationError ErrorCallback)
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
		UE_LOG(LogXsollaPayStation, Warning, TEXT("%s: request failed (%s): %s"), *VA_FUNC_LINE, *ErrorStr, *ResponseStr);
		ErrorCallback.ExecuteIfBound(StatusCode, ErrorStr);
		return true;
	}

	return false;
}

TSharedRef<IHttpRequest> UXsollaPayStationSubsystem::CreateHttpRequest(const FString& Url) const
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=paystation&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_PAYSTATION_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), TEXT("PAYSTATION"));
	HttpRequest->SetHeader(TEXT("X-SDK-V"), XSOLLA_PAYSTATION_VERSION);

	return HttpRequest;
}

void UXsollaPayStationSubsystem::LoadImageFromWeb(const FString& URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback)
{
	ImageLoader->LoadImage(URL, SuccessCallback, ErrorCallback);
}

#undef LOCTEXT_NAMESPACE