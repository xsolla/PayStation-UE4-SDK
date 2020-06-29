// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "XsollaPaymentsDataModel.h"

#include "XsollaPaymentsSubsystem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCheckPurchaseStatusSuccess, FXsollaPaymentsPurchaseStatus, PurchaseStatus);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnPaymentsError, int32, StatusCode, const FString&, ErrorMessage);

class UXsollaPaymentsImageLoader;

UCLASS()
class XSOLLAPAYMENTS_API UXsollaPaymentsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UXsollaPaymentsSubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	/** Open payment console for provided token */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Payments")
	void LaunchPaymentConsoleWithAccessToken(const FString& AccessToken, UUserWidget*& BrowserWidget);

	/** Open payment console for provided token */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Payments")
	void LaunchPaymentConsoleWithAccessData(FXsollaPaymentsAccessData accessData, UUserWidget*& BrowserWidget);

	/** Get pending payment URL to be opened in browser */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Payments")
	FString GetPendingPaymentsUrl() const;

	/** Check purchase status by its UUID */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|Payments", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void CheckPurchaseStatus(const FString& PurchaseUUID, const FOnCheckPurchaseStatusSuccess& SuccessCallback, const FOnPaymentsError& ErrorCallback);

protected:
	/** Check whether sandbox is enabled */
	bool IsSandboxEnabled() const;

	/** Open payment console in browser */
	void OpenBrowser(const FString& Url, UUserWidget*& BrowserWidget);

	void CheckPurchaseStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnCheckPurchaseStatusSuccess SuccessCallback, FOnPaymentsError ErrorCallback);

	/** Return true if error is happened */
	bool HandlePurchaseStatusError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnPaymentsError ErrorCallback);

private:
	/** Create HTTP request and add Xsolla API meta */
	TSharedRef<IHttpRequest> CreateHttpRequest(const FString& Url) const;

protected:
	/** Pending Payments URL to be opened in browser */
	FString PengindPaymentsUrl;

	static const FString PaymentEndpoint;
	static const FString SandboxPaymentEndpoint;

	/** Cached Xsolla Payments Project ID */
	int32 ProjectID;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;

	UPROPERTY()
	UXsollaPaymentsImageLoader* ImageLoader;

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