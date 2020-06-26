// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsBrowserModule.h"

#include "XsollaPaymentsBrowserAssetManager.h"

#include "IWebBrowserSingleton.h"
#include "Materials/Material.h"
#include "Modules/ModuleManager.h"
#include "WebBrowserModule.h"

class FXsollaPaymentsBrowserModule : public IXsollaPaymentsBrowserModule
{
public:
	virtual void StartupModule() override
	{
		if (PaymentsBrowserAssetMgr == nullptr)
		{
			PaymentsBrowserAssetMgr = NewObject<UXsollaPaymentsBrowserAssetManager>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
			PaymentsBrowserAssetMgr->LoadDefaultMaterials();

			IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
			if (WebBrowserSingleton)
			{
				WebBrowserSingleton->SetDefaultMaterial(PaymentsBrowserAssetMgr->GetDefaultMaterial());
				WebBrowserSingleton->SetDefaultTranslucentMaterial(PaymentsBrowserAssetMgr->GetDefaultTranslucentMaterial());
			}
		}
	}

	virtual void ShutdownModule() override
	{
	}

private:
	UXsollaPaymentsBrowserAssetManager* PaymentsBrowserAssetMgr;
};

IMPLEMENT_MODULE(FXsollaPaymentsBrowserModule, XsollaPaymentsBrowser);
