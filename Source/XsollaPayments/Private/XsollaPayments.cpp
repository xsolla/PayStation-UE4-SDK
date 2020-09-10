// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayments.h"

#include "XsollaPaymentsDefines.h"
#include "XsollaPaymentsSettings.h"

#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FXsollaPaymentsModule"

void FXsollaPaymentsModule::StartupModule()
{
	XsollaPaymentsSettings = NewObject<UXsollaPaymentsSettings>(GetTransientPackage(), "XsollaPaymentsSettings", RF_Standalone);
	XsollaPaymentsSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "XsollaPayments",
			LOCTEXT("RuntimeSettingsName", "Xsolla Payments"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Xsolla Payments"),
			XsollaPaymentsSettings);
	}

	UE_LOG(LogXsollaPayments, Log, TEXT("%s: XsollaPayments module started"), *VA_FUNC_LINE);
}

void FXsollaPaymentsModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "XsollaPayments");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		XsollaPaymentsSettings->RemoveFromRoot();
	}
	else
	{
		XsollaPaymentsSettings = nullptr;
	}
}

UXsollaPaymentsSettings* FXsollaPaymentsModule::GetSettings() const
{
	check(XsollaPaymentsSettings);
	return XsollaPaymentsSettings;
}

FString FXsollaPaymentsModule::GetTitleId() const
{
	check(XsollaPaymentsSettings);
	return XsollaPaymentsSettings->TitleID;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FXsollaPaymentsModule, XsollaPayments)

DEFINE_LOG_CATEGORY(LogXsollaPayments);