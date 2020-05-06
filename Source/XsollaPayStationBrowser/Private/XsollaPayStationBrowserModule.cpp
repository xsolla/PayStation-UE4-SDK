// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationBrowserModule.h"

#include "XsollaPayStationBrowserAssetManager.h"

#include "IWebBrowserSingleton.h"
#include "Materials/Material.h"
#include "Modules/ModuleManager.h"
#include "WebBrowserModule.h"

class FXsollaPayStationBrowserModule : public IXsollaPayStationBrowserModule
{
public:
	virtual void StartupModule() override
	{
		if (PayStationBrowserAssetMgr == nullptr)
		{
			PayStationBrowserAssetMgr = NewObject<UXsollaPayStationBrowserAssetManager>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
			PayStationBrowserAssetMgr->LoadDefaultMaterials();

			IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
			if (WebBrowserSingleton)
			{
				WebBrowserSingleton->SetDefaultMaterial(PayStationBrowserAssetMgr->GetDefaultMaterial());
				WebBrowserSingleton->SetDefaultTranslucentMaterial(PayStationBrowserAssetMgr->GetDefaultTranslucentMaterial());
			}
		}
	}

	virtual void ShutdownModule() override
	{
	}

private:
	UXsollaPayStationBrowserAssetManager* PayStationBrowserAssetMgr;
};

IMPLEMENT_MODULE(FXsollaPayStationBrowserModule, XsollaPayStationBrowser);
