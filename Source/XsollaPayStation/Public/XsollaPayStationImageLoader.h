// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Brushes/SlateDynamicImageBrush.h"
#include "CoreMinimal.h"
#include "Http.h"

#include "XsollaPayStationImageLoader.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnImageLoaded, FSlateBrush, ImageBrush);
DECLARE_DYNAMIC_DELEGATE(FOnImageLoadFailed);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRequestCompleted, bool);

UCLASS()
class XSOLLAPAYSTATION_API UXsollaPayStationImageLoader : public UObject
{
	GENERATED_BODY()

public:
	/** Load image from web by the given URL */
	UFUNCTION(BlueprintCallable, Category = "Xsolla | PayStation", meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void LoadImage(FString URL, const FOnImageLoaded& SuccessCallback, const FOnImageLoadFailed& ErrorCallback);

protected:
	void LoadImage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnImageLoaded SuccessCallback, FOnImageLoadFailed ErrorCallback);

private:
	FName GetCacheName(const FString& URL) const;

	/** Internal brushes cache */
	TMap<FString, TSharedPtr<FSlateDynamicImageBrush>> ImageBrushes;

	/** Internal cache for pending requests callbacks */
	TMap<FString, FOnRequestCompleted> PendingRequests;
};