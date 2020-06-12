// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

#include "XsollaPayStationSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCheckPurchaseStatusSuccess, FXsollaPayStationPurchaseStatus, PurchaseStatus);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnPayStationError, int32, StatusCode, const FString&, ErrorMessage);

class UXsollaPayStationImageLoader;

UCLASS()
class XSOLLAPAYSTATION_API UXsollaPayStationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UXsollaPayStationSubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	/** Open payment console for provided token */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PayStation")
	void LaunchPaymentConsole(const FString& PaymentToken, UUserWidget*& BrowserWidget);

	/** Get pending PayStation URL to be opened in browser */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PayStation")
	FString GetPendingPayStationUrl() const;

	/** Check purchase status by its UUID */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PayStation", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckPurchaseStatus(const FString& PurchaseUUID, const FOnCheckPurchaseStatusSuccess& SuccessCallback, const FOnPayStationError& ErrorCallback) const;

protected:
	/** Check whether sandbox is enabled */
	bool IsSandboxEnabled() const;

	void CheckPurchaseStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCheckPurchaseStatusSuccess SuccessCallback, FOnPayStationError ErrorCallback);

	/** Return true if error is happened */
	bool HandlePurchaseStatusError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnPayStationError ErrorCallback);

private:
	/** Create HTTP request and add Xsolla API meta */
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url) const;

protected:
	/** Pending PayStation URL to be opened in browser */
	FString PengindPayStationUrl;

	static const FString PaymentEndpoint;
	static const FString SandboxPaymentEndpoint;

	/** Cached Xsolla PayStation Project ID */
	FString ProjectID;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;

	UPROPERTY()
	UXsollaPayStationImageLoader* ImageLoader;

public:
	/** Async load image from the web
	 *
	 * @param URL Address of image to be downloaded.
	 * @param SuccessCallback Callback function called after successful image download.
	 * @param ErrorCallback Callback function called after request resulted with an error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Store", meta = (AutoCreateRefTerm = "ErrorCallback"))
	void LoadImageFromWeb(const FString& URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback);
};