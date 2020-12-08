// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPlayFabSubsystem.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "XsollaPayments.h"
#include "XsollaPaymentsDefines.h"
#include "XsollaPaymentsLibrary.h"
#include "Interfaces/IHttpResponse.h"

const FString UXsollaPlayFabSubsystem::LoginEndpoint(TEXT("/Client/LoginWithPlayFab"));
const FString UXsollaPlayFabSubsystem::CatalogItemsEndpoint(TEXT("/Client/GetCatalogItems"));
const FString UXsollaPlayFabSubsystem::RegisterEndpoint(TEXT("/Client/RegisterPlayFabUser"));
const FString UXsollaPlayFabSubsystem::RecoveryEmailEndpoint(TEXT("/Client/SendAccountRecoveryEmail"));
const FString UXsollaPlayFabSubsystem::UserInventoryEndpoint(TEXT("/Client/GetUserInventory"));
const FString UXsollaPlayFabSubsystem::PurchaseItemEndpoint(TEXT("/Client/PurchaseItem"));
const FString UXsollaPlayFabSubsystem::StartPurchaseEndpoint(TEXT("/Client/StartPurchase"));
const FString UXsollaPlayFabSubsystem::GetPurchaseEndpoint(TEXT("/Client/GetPurchase"));
const FString UXsollaPlayFabSubsystem::ConsumeItemEndpoint(TEXT("/Client/ConsumeItem"));
const FString UXsollaPlayFabSubsystem::ExecuteCloudScriptEndpoint(TEXT("/Client/ExecuteCloudScript"));


FXsollaLoginResult UXsollaPlayFabSubsystem::LoginData;

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UXsollaPlayFabSubsystem::CreateHttpRequest(
	const FString& Url, const EXsollaPlayFabRequestVerb Verb, const FString& Content, const FString& AuthToken)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(
		FString::Printf(TEXT("https://%s.playfabapi.com%s"),
		                *FXsollaPaymentsModule::Get().GetSettings()->GetTitleID(), *Url));

	switch (Verb)
	{
	case EXsollaPlayFabRequestVerb::GET:
		HttpRequest->SetVerb(TEXT("GET"));

		// Check that we don't provide content with GET request
		if (!Content.IsEmpty())
		{
			UE_LOG(LogXsollaPayments, Warning,
			       TEXT("%s: Request content is not empty for GET request. Maybe you should use POST one?"),
			       *VA_FUNC_LINE);
		}
		break;

	case EXsollaPlayFabRequestVerb::POST:
		HttpRequest->SetVerb(TEXT("POST"));
		break;

	case EXsollaPlayFabRequestVerb::PUT:
		HttpRequest->SetVerb(TEXT("PUT"));
		break;

	case EXsollaPlayFabRequestVerb::DELETE:
		HttpRequest->SetVerb(TEXT("DELETE"));
		break;

	case EXsollaPlayFabRequestVerb::PATCH:
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

bool UXsollaPlayFabSubsystem::HandleRequestError(
	FHttpRequestPtr HttpRequest, const FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnAnyError ErrorCallback)
{
	if (bSucceeded && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		return false;
	}

	if (HttpResponse == nullptr)
	{
		FXsollaApiErrorWrapper Result;
		Result.ErrorMessage = TEXT("HttpResponse is not valid");
		UE_LOG(LogXsollaPayments, Warning, TEXT("%s: HttpResponse is not valid"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(Result);
		return true;
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

void UXsollaPlayFabSubsystem::LoginWithPlayFab(
	const FXsollaLoginRequest Request, const FOnLoginSuccess& SuccessCallback, const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Request.Password.IsEmpty())
	{
		JsonObject->SetStringField(TEXT("Password"), Request.Password);
	}
	JsonObject->SetStringField(TEXT("TitleId"), FXsollaPaymentsModule::Get().GetSettings()->GetTitleID());
	if (!Request.Username.IsEmpty())
	{
		JsonObject->SetStringField(TEXT("Username"), Request.Username);
	}

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		LoginEndpoint, EXsollaPlayFabRequestVerb::POST, Content);
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
	JsonObject->SetStringField(TEXT("TitleId"), FXsollaPaymentsModule::Get().GetSettings()->GetTitleID());

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		RegisterEndpoint, EXsollaPlayFabRequestVerb::POST, Content);
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
	JsonObject->SetStringField(TEXT("TitleId"), FXsollaPaymentsModule::Get().GetSettings()->GetTitleID());

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		RecoveryEmailEndpoint, EXsollaPlayFabRequestVerb::POST, Content);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::SendAccountRecoveryEmail_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::GetUserInventory(
	const FOnInventoryReceived& SuccessCallback, const FOnAnyError& ErrorCallback)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		UserInventoryEndpoint, EXsollaPlayFabRequestVerb::POST, "", LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::GetUserInventory_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::GetCatalogItems(
	const FString CatalogVersion, const FOnCatalogReceived& SuccessCallback, const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("CatalogVersion"), CatalogVersion);

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		CatalogItemsEndpoint, EXsollaPlayFabRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::GetCatalogItems_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::PurchaseItem(
	const FXsollaPurchaseItemRequest Request, const FOnPurchaseItemSuccess& SuccessCallback,
	const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Request.CatalogVersion.IsEmpty()) JsonObject->SetStringField(TEXT("CatalogVersion"), Request.CatalogVersion);
	if (!Request.CharacterId.IsEmpty()) JsonObject->SetStringField(TEXT("CharacterId"), Request.CharacterId);
	if (!Request.ItemId.IsEmpty()) JsonObject->SetStringField(TEXT("ItemId"), Request.ItemId);
	JsonObject->SetNumberField(TEXT("Price"), Request.Price);
	if (!Request.StoreId.IsEmpty()) JsonObject->SetStringField(TEXT("StoreId"), Request.StoreId);
	if (!Request.VirtualCurrency.IsEmpty()) JsonObject->
		SetStringField(TEXT("VirtualCurrency"), Request.VirtualCurrency);

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		PurchaseItemEndpoint, EXsollaPlayFabRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::PurchaseItem_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::StartPurchase(
	const FXsollaStartPurchaseRequest Request, const FOnStartPurchaseSuccess& SuccessCallback,
	const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Request.CatalogVersion.IsEmpty()) JsonObject->SetStringField(TEXT("CatalogVersion"), Request.CatalogVersion);
	if (!Request.StoreId.IsEmpty()) JsonObject->SetStringField(TEXT("StoreId"), Request.StoreId);

	TArray<TSharedPtr<FJsonValue>> Items = TArray<TSharedPtr<FJsonValue>>();
	for (auto Elem : Request.Items)
	{
		TSharedPtr<FJsonObject> Item = MakeShareable(new FJsonObject());
		Item->SetStringField(TEXT("ItemId"), Elem.ItemId);
		Item->SetNumberField(TEXT("Quantity"), Elem.Quantity);
		for (auto SubElem : Elem.UpgradeFromItems)
		{
			if (!SubElem.IsEmpty())
			{
				JsonObject->SetStringField(TEXT("StoreId"), SubElem);
			}
		}
		Items.Push(MakeShareable(new FJsonValueObject(Item)));
	}

	JsonObject->SetArrayField(TEXT("Items"), Items);

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		StartPurchaseEndpoint, EXsollaPlayFabRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::StartPurchase_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::GetPurchase(
	const FXsollaGetPurchaseRequest Request, const FOnGetPurchaseSuccess& SuccessCallback,
	const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	JsonObject->SetStringField(TEXT("OrderId"), Request.OrderId);

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		GetPurchaseEndpoint, EXsollaPlayFabRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::GetPurchase_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::ConsumeItem(
	const FXsollaConsumeItemRequest Request, const FOnConsumeItemSuccess& SuccessCallback,
	const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Request.CharacterId.IsEmpty()) JsonObject->SetStringField(TEXT("CharacterId"), Request.CharacterId);
	JsonObject->SetNumberField(TEXT("ConsumeCount"), Request.ConsumeCount);
	if (!Request.ItemInstanceId.IsEmpty()) JsonObject->SetStringField(TEXT("ItemInstanceId"), Request.ItemInstanceId);

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		ConsumeItemEndpoint, EXsollaPlayFabRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::ConsumeItem_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::ExecuteCloudScript(
	const FXsollaExecuteCloudScriptRequest Request, const FOnExecuteCloudScriptSuccess& SuccessCallback,
	const FOnAnyError& ErrorCallback)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (!Request.FunctionName.IsEmpty()) JsonObject->SetStringField(TEXT("FunctionName"), Request.FunctionName);
	JsonObject->SetBoolField(TEXT("GeneratePlayStreamEvent"), Request.GeneratePlayStreamEvent);

	const TSharedPtr<FJsonObject> FunctionParameter = FJsonObjectConverter::UStructToJsonObject(
		Request.FunctionParameter);

	JsonObject->SetObjectField(TEXT("FunctionParameter"), FunctionParameter);

	const UEnum* EnumType = FindObject<UEnum>(ANY_PACKAGE, TEXT("EXsollaCloudScriptRevisionOption"));
	const FString RevisionSelection = EnumType->GetNameStringByIndex(
		static_cast<uint32>(Request.RevisionSelection));
	JsonObject->SetStringField(TEXT("RevisionSelection"), RevisionSelection);
	JsonObject->SetNumberField(TEXT("SpecificRevision"), Request.SpecificRevision);

	FString Content;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateHttpRequest(
		ExecuteCloudScriptEndpoint, EXsollaPlayFabRequestVerb::POST, Content, LoginData.SessionTicket);
	HttpRequest->OnProcessRequestComplete().BindUObject(
		this, &UXsollaPlayFabSubsystem::ExecuteCloudScript_HttpRequestComplete,
		SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPlayFabSubsystem::LoginWithPlayFab_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnLoginSuccess SuccessCallback, FOnAnyError ErrorCallback)
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
		FXsollaLoginResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(Data.ToSharedRef(), FXsollaLoginResult::StaticStruct(),
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

void UXsollaPlayFabSubsystem::StartPurchase_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnStartPurchaseSuccess SuccessCallback, FOnAnyError ErrorCallback)
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

void UXsollaPlayFabSubsystem::ConsumeItem_HttpRequestComplete(FHttpRequestPtr HttpRequest,
                                                              FHttpResponsePtr HttpResponse, bool bSucceeded,
                                                              FOnConsumeItemSuccess SuccessCallback,
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
		FXsollaConsumeItemResult Result;
		const TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField("data");
		if (FJsonObjectConverter::JsonObjectToUStruct(
			Data.ToSharedRef(), FXsollaConsumeItemResult::StaticStruct(), &Result))
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
