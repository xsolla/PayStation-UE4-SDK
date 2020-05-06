// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationBrowserAssetManager.h"

#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
#include "WebBrowserTexture.h"
#endif

UXsollaPayStationBrowserAssetManager::UXsollaPayStationBrowserAssetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultMaterial(FString(TEXT("/XsollaPayStation/Browser/M_WebTexture.M_WebTexture")))
	, DefaultTranslucentMaterial(FString(TEXT("/XsollaPayStation/Browser/M_WebTexture_Translucent.M_WebTexture_Translucent")))
{
#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
	// Add a hard reference to UXsollaPayStationBrowserTexture, without this the WebBrowserTexture DLL never gets loaded on Windows.
	UWebBrowserTexture::StaticClass();
#endif
};

void UXsollaPayStationBrowserAssetManager::LoadDefaultMaterials()
{
	DefaultMaterial.LoadSynchronous();
	DefaultTranslucentMaterial.LoadSynchronous();
}

UMaterial* UXsollaPayStationBrowserAssetManager::GetDefaultMaterial() const
{
	return DefaultMaterial.Get();
}

UMaterial* UXsollaPayStationBrowserAssetManager::GetDefaultTranslucentMaterial() const
{
	return DefaultTranslucentMaterial.Get();
}
