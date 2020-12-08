// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsImageLoader.h"
#include "XsollaPaymentsDefines.h"
#include "Framework/Application/SlateApplication.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/SecureHash.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FXsollaPayStatiomModule"

void UXsollaPaymentsImageLoader::LoadImage(
	FString URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback)
{
	UE_LOG(LogXsollaPayments, VeryVerbose, TEXT("%s: Loading image from: %s"), *VA_FUNC_LINE, *URL);

	const FString ResourceId = GetCacheName(URL).ToString();
	if (ImageBrushes.Contains(ResourceId))
	{
		UE_LOG(LogXsollaPayments, VeryVerbose, TEXT("%s: Loaded from cache: %s"), *VA_FUNC_LINE, *ResourceId);
		SuccessCallback.ExecuteIfBound(*ImageBrushes.Find(ResourceId)->Get(), URL);
	}
	else
	{
		if (PendingRequests.Contains(ResourceId))
		{
			PendingRequests[ResourceId].AddLambda([=](bool IsCompleted)
			{
				if (IsCompleted)
				{
					UE_LOG(LogXsollaPayments, VeryVerbose, TEXT("%s: Loaded from cache: %s"), *VA_FUNC_LINE, *ResourceId);
					SuccessCallback.ExecuteIfBound(*ImageBrushes.Find(ResourceId)->Get(), URL);
				}
				else
				{
					UE_LOG(LogXsollaPayments, Error, TEXT("%s: Failed to get image"), *VA_FUNC_LINE);
					ErrorCallback.ExecuteIfBound(URL);
				}
			});
		}
		else
		{
			const FOnRequestCompleted ImageLoadingCompletedDelegate;
			PendingRequests.Add(ResourceId, ImageLoadingCompletedDelegate);

			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

			HttpRequest->OnProcessRequestComplete().BindUObject(
				this, &UXsollaPaymentsImageLoader::LoadImage_HttpRequestComplete, SuccessCallback, ErrorCallback);
			HttpRequest->SetURL(URL);
			HttpRequest->SetVerb(TEXT("GET"));

			HttpRequest->ProcessRequest();
		}
	}
}

void UXsollaPaymentsImageLoader::LoadImage_HttpRequestComplete(
	FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded,
	FOnImageLoaded SuccessCallback, FOnImageLoadFailed ErrorCallback)
{
	const FName ResourceName = GetCacheName(HttpRequest->GetURL());

	if (bSucceeded && HttpResponse.IsValid())
	{
		const TArray<uint8>& ImageData = HttpResponse->GetContent();

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(
			FName("ImageWrapper"));
		const EImageFormat ImageType = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageType);

		if (!ImageWrapper.IsValid())
		{
			UE_LOG(LogXsollaPayments, Error, TEXT("%s: Invalid image wrapper"), *VA_FUNC_LINE);
		}
		else if (ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
		{
			const int32 BytesPerPixel = ImageWrapper->GetBitDepth();
			TArray<uint8> RawData;

			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, BytesPerPixel, RawData) && RawData.Num() > 0)
			{
				if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(
					ResourceName, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), RawData))
				{
					TSharedPtr<FSlateDynamicImageBrush> ImageBrush = 
						MakeShareable(new FSlateDynamicImageBrush(
							ResourceName, FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight())));
					ImageBrushes.Add(ResourceName.ToString(), ImageBrush);

					SuccessCallback.ExecuteIfBound(*ImageBrush.Get(), HttpRequest->GetURL());

					PendingRequests[ResourceName.ToString()].Broadcast(true);
					PendingRequests.Remove(ResourceName.ToString());

					return;
				}
				else
				{
					UE_LOG(LogXsollaPayments, Error, TEXT("%s: Can't generate resource"), *VA_FUNC_LINE);
				}
			}
			else
			{
				UE_LOG(LogXsollaPayments, Error, TEXT("%s: Can't get raw data"), *VA_FUNC_LINE);
			}
		}
		else
		{
			UE_LOG(LogXsollaPayments, Error, TEXT("%s: Can't load compressed data"), *VA_FUNC_LINE);
		}
	}
	else
	{
		UE_LOG(LogXsollaPayments, Error, TEXT("%s: Failed to download image"), *VA_FUNC_LINE);
	}

	ErrorCallback.ExecuteIfBound(HttpRequest->GetURL());

	if (PendingRequests.Contains(ResourceName.ToString()))
	{
		PendingRequests[ResourceName.ToString()].Broadcast(false);
		PendingRequests.Remove(ResourceName.ToString());
	}
}

FName UXsollaPaymentsImageLoader::GetCacheName(const FString& URL) const
{
	return FName(*FString::Printf(TEXT("XsollaPaymentsImage_%s"), *FMD5::HashAnsiString(*URL)));
}

#undef LOCTEXT_NAMESPACE
