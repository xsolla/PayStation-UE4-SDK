// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "XsollaPlayFabDataModel.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "XsollaPlayFabSubsystem.generated.h"

UENUM(BlueprintType)
enum class EXsollaPlayFabRequestVerb : uint8
{
	GET,
    POST,
    PUT,
    DELETE,
    PATCH
};

/**
 *  Class wrapper for working with PlayFab API.
 */
UCLASS()
class XSOLLAPAYMENTS_API UXsollaPlayFabSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	// Delegates
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLoginSuccess, FXsollaLoginResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRegisterSuccess, FXsollaRegisterUserResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRecoveryEmailSuccess, FXsollaRecoveryEmailResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCatalogReceived, FXsollaCatalogItemsResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInventoryReceived, FXsollaUserInventoryResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPurchaseItemSuccess, FXsollaPurchaseItemResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStartPurchaseSuccess, FXsollaStartPurchaseResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetPurchaseSuccess, FXsollaGetPurchaseResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnConsumeItemSuccess, FXsollaConsumeItemResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnExecuteCloudScriptSuccess, FXsollaExecuteCloudScriptResult, Result);

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAnyError, FXsollaApiErrorWrapper, Error);

	// Endpoints
	static const FString LoginEndpoint;
	static const FString RegisterEndpoint;
	static const FString RecoveryEmailEndpoint;
	static const FString CatalogItemsEndpoint;
	static const FString UserInventoryEndpoint;
	static const FString PurchaseItemEndpoint;
	static const FString StartPurchaseEndpoint;
	static const FString GetPurchaseEndpoint;
	static const FString ConsumeItemEndpoint;
	static const FString ExecuteCloudScriptEndpoint;

	static FXsollaLoginResult LoginData;

	// Helpers

	/** Create http request */
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(
		const FString& Url, const EXsollaPlayFabRequestVerb Verb = EXsollaPlayFabRequestVerb::GET,
		const FString& Content = FString(), const FString& AuthToken = FString());

	bool HandleRequestError(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	                        FOnAnyError ErrorCallback);

public:
	// Methods

	/**
	* Signs the user into the PlayFab account, returning a session identifier that can subsequently be used for API calls
	* which require an authenticated user. Unlike most other login API calls, LoginWithPlayFab doesn't permit the creation of
	* new accounts via the CreateAccountFlag. Username/Password credentials may be used to create accounts via
	* RegisterPlayFabUser, or added to existing accounts using AddUsernamePassword.
	*/
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Login", meta = (AutoCreateRefTerm =
		"SuccessCallback, ErrorCallback"
	))
	void LoginWithPlayFab(const FXsollaLoginRequest Request, const FOnLoginSuccess& SuccessCallback,
	                      const FOnAnyError& ErrorCallback);

	/**
	 * Registers a new Playfab user account, returning a session identifier that can subsequently be used for API calls which require an authenticated user.
	 * You must supply either a username or an email address.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Register", meta = (AutoCreateRefTerm =
		"SuccessCallback, ErrorCallback"))
	void RegisterPlayFabUser(const FXsollaRegisterRequest Request, const FOnRegisterSuccess& SuccessCallback,
	                         const FOnAnyError& ErrorCallback);

	/** Forces sending of the email to the registered email address for the user's account with a link allowing the user to change the password.
	 *  If an account recovery email template ID is provided, an email using the custom email template will be used.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Recovery", meta = (AutoCreateRefTerm =
		"SuccessCallback, ErrorCallback"))
	void SendAccountRecoveryEmail(const FXsollaRecoveryEmailRequest Request,
	                              const FOnRecoveryEmailSuccess& SuccessCallback, const FOnAnyError& ErrorCallback);

	/** Retrieves the user's current inventory of virtual goods. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Shop", meta = (AutoCreateRefTerm =
			"SuccessCallback, ErrorCallback")
	)
	void GetUserInventory(const FOnInventoryReceived& SuccessCallback,
	                      const FOnAnyError& ErrorCallback);

	/** Retrieves the specified version of the title's catalog of virtual goods including all defined properties.
	 * @param CatalogVersion - Requested catalog. If null, uses the default catalog.
	 * @param SuccessCallback - Callback which will be executed after success.
	 * @param ErrorCallback - Callback which will be executed if an error occurs.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Shop",
		meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetCatalogItems(const FString CatalogVersion, const FOnCatalogReceived& SuccessCallback,
	                     const FOnAnyError& ErrorCallback);

	/** Buys a single item with virtual currency. You must specify both the virtual currency that is used for the purchase and
	 *  the price expected by the player.
	 *  This lets the server fail the purchase if the price has changed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Shop",
		meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void PurchaseItem(const FXsollaPurchaseItemRequest Request, const FOnPurchaseItemSuccess& SuccessCallback,
	                  const FOnAnyError& ErrorCallback);

	/** Creates an order for a list of items from the title catalog. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Shop",
		meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void StartPurchase(const FXsollaStartPurchaseRequest Request, const FOnStartPurchaseSuccess& SuccessCallback,
	                   const FOnAnyError& ErrorCallback);

	/** Retrieves a purchase along with its current PlayFab status.
	 *  Returns inventory items from the purchase that are still active.
	 */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Shop",
		meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetPurchase(const FXsollaGetPurchaseRequest Request, const FOnGetPurchaseSuccess& SuccessCallback,
	                 const FOnAnyError& ErrorCallback);

	/** Consumes uses of a consumable item. When all uses are consumed, it will be removed from the player's inventory. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Shop",
		meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ConsumeItem(const FXsollaConsumeItemRequest Request, const FOnConsumeItemSuccess& SuccessCallback,
	                 const FOnAnyError& ErrorCallback);

	/** Executes a CloudScript function, with the 'currentPlayerId' set to the PlayFab ID of the authenticated player. */
	UFUNCTION(BlueprintCallable, Category = "Xsolla|PlayFab Wrapper|Shop",
		meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void ExecuteCloudScript(const FXsollaExecuteCloudScriptRequest Request,
	                        const FOnExecuteCloudScriptSuccess& SuccessCallback,
	                        const FOnAnyError& ErrorCallback);

private:
	// Completes

	void LoginWithPlayFab_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                          bool bSucceeded, FOnLoginSuccess SuccessCallback,
	                                          FOnAnyError ErrorCallback);

	void RegisterPlayFabUser_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                             bool bSucceeded, FOnRegisterSuccess SuccessCallback,
	                                             FOnAnyError ErrorCallback);

	void SendAccountRecoveryEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                                  bool bSucceeded, FOnRecoveryEmailSuccess SuccessCallback,
	                                                  FOnAnyError ErrorCallback);

	void GetUserInventory_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                          bool bSucceeded, FOnInventoryReceived SuccessCallback,
	                                          FOnAnyError ErrorCallback);

	void GetCatalogItems_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                         bool bSucceeded, FOnCatalogReceived SuccessCallback,
	                                         FOnAnyError ErrorCallback);

	void PurchaseItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                      bool bSucceeded, FOnPurchaseItemSuccess SuccessCallback,
	                                      FOnAnyError ErrorCallback);

	void StartPurchase_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                       bool bSucceeded, FOnStartPurchaseSuccess SuccessCallback,
	                                       FOnAnyError ErrorCallback);

	void GetPurchase_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                     bool bSucceeded, FOnGetPurchaseSuccess SuccessCallback,
	                                     FOnAnyError ErrorCallback);

	void ConsumeItem_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                     bool bSucceeded, FOnConsumeItemSuccess SuccessCallback,
	                                     FOnAnyError ErrorCallback);

	void ExecuteCloudScript_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
	                                            bool bSucceeded, FOnExecuteCloudScriptSuccess SuccessCallback,
	                                            FOnAnyError ErrorCallback);
};
