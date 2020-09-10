// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPlayFabSubsystem.h"
#include "JsonObjectConverter.h"
#include "XsollaPayments.h"
#include "XsollaPaymentsDefines.h"
#include "XsollaPaymentsLibrary.h"
#include "XsollaPlayFabModels.h"
#include "Interfaces/IHttpResponse.h"

const FString UXsollaPlayFabSubsystem::LoginEndpoint(TEXT("/Client/LoginWithPlayFab"));
const FString UXsollaPlayFabSubsystem::CatalogItemsEndpoint(TEXT("/Client/GetCatalogItems"));
const FString UXsollaPlayFabSubsystem::RegisterEndpoint(TEXT("/Client/RegisterPlayFabUser"));
const FString UXsollaPlayFabSubsystem::RecoveryEmailEndpoint(TEXT("/Client/SendAccountRecoveryEmail"));
const FString UXsollaPlayFabSubsystem::UserInventoryEndpoint(TEXT("/Client/GetUserInventory"));
const FString UXsollaPlayFabSubsystem::PurchaseItemEndpoint(TEXT("/Client/PurchaseItem"));
const FString UXsollaPlayFabSubsystem::StartPurchaseEndpoint(TEXT("/Client/StartPurchase"));
const FString UXsollaPlayFabSubsystem::GetPurchaseEndpoint(TEXT("/Client/GetPurchase"));
const FString UXsollaPlayFabSubsystem::ExecuteCloudScriptEndpoint(TEXT("/Client/ExecuteCloudScript"));


FXsollaClientLoginResult UXsollaPlayFabSubsystem::LoginData;

void UXsollaPlayFabSubsystem::LoginWithPlayFab(
	const FXsollaClientLoginRequest Request, const FOnLoginSuccess& SuccessCallback, const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Request.Password.IsEmpty())
	{
		JsonObject->SetStringField(TEXT("Password"), Request.Password);
	}
	JsonObject->SetStringField(TEXT("TitleId"), FXsollaPaymentsModule::Get().GetTitleId());
	if (!Request.Username.IsEmpty())
	{
		JsonObject->SetStringField(TEXT("Username"), Request.Username);
	}

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		LoginEndpoint, EXsollaLoginRequestVerb::POST, Content);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::LoginWithPlayFab_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::RegisterPlayFabUser(
	const FXsollaRegisterRequest Request, const FOnRegisterSuccess& SuccessCallback, const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	JsonObject->SetStringField(TEXT("Username"), Request.Username);
	JsonObject->SetStringField(TEXT("Password"), Request.Password);
	JsonObject->SetStringField(TEXT("Email"), Request.Email);
	if (!Request.DisplayName.IsEmpty())
	{
		JsonObject->SetStringField(TEXT("DisplayName"), Request.DisplayName);
	}
	JsonObject->SetBoolField(TEXT("RequireBothUsernameAndEmail"), false);
	JsonObject->SetStringField(TEXT("TitleId"), FXsollaPaymentsModule::Get().GetTitleId());

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		RegisterEndpoint, EXsollaLoginRequestVerb::POST, Content);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::RegisterPlayFabUser_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::SendAccountRecoveryEmail(
	const FXsollaRecoveryEmailRequest Request, const FOnRecoveryEmailSuccess& SuccessCallback,
	const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	JsonObject->SetStringField(TEXT("Email"), Request.Email);
	JsonObject->SetStringField(TEXT("TitleId"), FXsollaPaymentsModule::Get().GetTitleId());

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		RecoveryEmailEndpoint, EXsollaLoginRequestVerb::POST, Content);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::SendAccountRecoveryEmail_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::GetUserInventory(
	UObject* CustomTags, const FOnInventoryReceived& SuccessCallback, const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	// JsonObject->SetObjectField(TEXT("CustomTags"), MakeShared<FJsonObject>());
	JsonObject->SetStringField(TEXT("Test"), "Test");

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		UserInventoryEndpoint, EXsollaLoginRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::GetUserInventory_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::RegisterPlayFabUser_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnRegisterSuccess SuccessCallback,
	FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaRegisterUserResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(Data.ToSharedRef(),
		                                              FXsollaRegisterUserResult::StaticStruct(), &Result))
		{
			LoginData.EntityToken = Result.EntityToken;
			LoginData.SessionTicket = Result.SessionTicket;
			LoginData.PlayFabId = Result.PlayFabId;
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::SendAccountRecoveryEmail_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnRecoveryEmailSuccess SuccessCallback, FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaRecoveryEmailResult Result;
		if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(),
		                                              FXsollaRecoveryEmailResult::StaticStruct(), &Result))
		{
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::GetUserInventory_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnInventoryReceived SuccessCallback, FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaUserInventoryResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(
			Data.ToSharedRef(), FXsollaUserInventoryResult::StaticStruct(), &Result))
		{
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::PurchaseItem_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnPurchaseItemSuccess SuccessCallback,
	FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaPurchaseItemResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(
			Data.ToSharedRef(), FXsollaPurchaseItemResult::StaticStruct(), &Result))
		{
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::StartPurchase_HttpRequestComplete(FHttpRequestPtr HttpRequest,
                                                                FHttpResponsePtr HttpResponse, bool bSucceeded,
                                                                FOnStartPurchaseSuccess SuccessCallback,
                                                                FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaStartPurchaseResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(
			Data.ToSharedRef(), FXsollaStartPurchaseResult::StaticStruct(), &Result))
		{
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::GetPurchase_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnGetPurchaseSuccess SuccessCallback,
	FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaGetPurchaseResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(
			Data.ToSharedRef(), FXsollaGetPurchaseResult::StaticStruct(), &Result))
		{
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::ExecuteCloudScript_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnExecuteCloudScriptSuccess SuccessCallback, FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaExecuteCloudScriptResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(
			Data.ToSharedRef(), FXsollaExecuteCloudScriptResult::StaticStruct(), &Result))
		{
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::GetCatalogItems(const FString CatalogVersion, const FOnCatalogReceived& SuccessCallback,
                                              const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("CatalogVersion", CatalogVersion);

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		CatalogItemsEndpoint, EXsollaLoginRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::GetCatalogItems_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::PurchaseItem(
	const FXsollaPurchaseItemRequest Request, const FOnPurchaseItemSuccess& SuccessCallback,
	const FOnAnyError& ErrorCallback)
{
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(Request, Content);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		PurchaseItemEndpoint, EXsollaLoginRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::PurchaseItem_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::StartPurchase(const FXsollaStartPurchaseRequest Request,
                                            const FOnStartPurchaseSuccess& SuccessCallback,
                                            const FOnAnyError& ErrorCallback)
{
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(Request, Content);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		StartPurchaseEndpoint, EXsollaLoginRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::StartPurchase_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::GetPurchase(const FXsollaGetPurchaseRequest Request,
                                          const FOnGetPurchaseSuccess& SuccessCallback,
                                          const FOnAnyError& ErrorCallback)
{
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(Request, Content);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		GetPurchaseEndpoint, EXsollaLoginRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::GetPurchase_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::ExecuteCloudScript(const FXsollaExecuteCloudScriptRequest Request,
                                                 const FOnExecuteCloudScriptSuccess& SuccessCallback,
                                                 const FOnAnyError& ErrorCallback)
{
	FString Content;
	FJsonObjectConverter::UStructToJsonObjectString(Request, Content);

	TSharedRef<IHttpRequest> HttpRequest = UXsollaPaymentsLibrary::CreateHttpRequest(
		ExecuteCloudScriptEndpoint, EXsollaLoginRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::ExecuteCloudScript_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::LoginWithPlayFab_HttpRequestComplete(FHttpRequestPtr HttpRequest,
                                                                   FHttpResponsePtr HttpResponse, bool bSucceeded,
                                                                   FOnLoginSuccess SuccessCallback,
                                                                   FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaClientLoginResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(Data.ToSharedRef(), FXsollaClientLoginResult::StaticStruct(),
		                                              &Result))
		{
			LoginData = Result;
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

void UXsollaPlayFabSubsystem::GetCatalogItems_HttpRequestComplete(
	const FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse, const bool bSucceeded,
	FOnCatalogReceived SuccessCallback, const FOnAnyError ErrorCallback)
{
	if (HandleRequestError(HttpRequest, HttpResponse, bSucceeded, ErrorCallback))
	{
		return;
	}

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Response %s"), *VA_FUNC_LINE, *ResponseStr);

	FString ErrorStr;

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FXsollaCatalogItemsResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(Data.ToSharedRef(), FXsollaCatalogItemsResult::StaticStruct(),
		                                              &Result))
		{
			if (!SuccessCallback.ExecuteIfBound(Result))
			{
				UE_LOG(LogXsollaPayments, Warning, TEXT("%s: SuccessCallback not found, nothing executed"),
				       *VA_FUNC_LINE);
			}
			return;
		}

		ErrorStr = FString::Printf(TEXT("Can't process response json"));
	}
	else
	{
		ErrorStr = FString::Printf(TEXT("Can't deserialize response json: %s"), *ResponseStr);
	}

	FXsollaApiErrorWrapper Error;
	Error.ErrorMessage = ErrorStr;
	if (!ErrorCallback.ExecuteIfBound(Error))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
}

bool UXsollaPlayFabSubsystem::HandleRequestError(FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse,
                                                 const bool bSucceeded, FOnAnyError ErrorCallback)
{
	if (bSucceeded && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		return false;
	}

	FString ErrorStr;

	const FString ResponseStr = HttpResponse->GetContentAsString();
	UE_LOG(LogXsollaPayments, Warning, TEXT("%s: Error code: %d Response %s"), *VA_FUNC_LINE,
	       HttpResponse->GetResponseCode(), *ResponseStr);

	FXsollaApiErrorWrapper Result;

	if (bSucceeded && HttpResponse.IsValid())
	{
		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseStr);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FXsollaApiErrorWrapper::StaticStruct(),
			                                          &Result);
		}
		else
		{
			ErrorStr = TEXT("Can't deserialize error json");
		}
	}
	else
	{
		ErrorStr = TEXT("No response");
	}

	if (!ErrorStr.IsEmpty())
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: request failed (%s): %s"), *VA_FUNC_LINE, *ErrorStr, *ResponseStr);
	}

	if (!ErrorCallback.ExecuteIfBound(Result))
	{
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: ErrorCallback not found, nothing executed"), *VA_FUNC_LINE);
	}
	return true;
}
