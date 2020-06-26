// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsBrowserAssetManager.h"

#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
#include "WebBrowserTexture.h"
#endif

UXsollaPaymentsBrowserAssetManager::UXsollaPaymentsBrowserAssetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultMaterial(FString(TEXT("/XsollaPayments/Browser/M_WebTexture.M_WebTexture")))
	, DefaultTranslucentMaterial(FString(TEXT("/XsollaPayments/Browser/M_WebTexture_Translucent.M_WebTexture_Translucent")))
{
#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
	// Add a hard reference to UXsollaPaymentsBrowserTexture, without this the WebBrowserTexture DLL never gets loaded on Windows.
	UWebBrowserTexture::StaticClass();
#endif
};

void UXsollaPaymentsBrowserAssetManager::LoadDefaultMaterials()
{
	DefaultMaterial.LoadSynchronous();
	DefaultTranslucentMaterial.LoadSynchronous();
}

UMaterial* UXsollaPaymentsBrowserAssetManager::GetDefaultMaterial() const
{
	return DefaultMaterial.Get();
}

UMaterial* UXsollaPaymentsBrowserAssetManager::GetDefaultTranslucentMaterial() const
{
	return DefaultTranslucentMaterial.Get();
}
