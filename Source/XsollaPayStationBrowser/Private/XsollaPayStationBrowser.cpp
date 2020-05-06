// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationBrowser.h"

#include "Async/TaskGraphInterfaces.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "XsollaPayStationBrowser"

UXsollaPayStationBrowser::UXsollaPayStationBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UXsollaPayStationBrowser::LoadURL(FString NewURL)
{
	if (PayStationBrowserWidget.IsValid())
	{
		return PayStationBrowserWidget->LoadURL(NewURL);
	}
}

FString UXsollaPayStationBrowser::GetUrl() const
{
	if (PayStationBrowserWidget.IsValid())
	{
		return PayStationBrowserWidget->GetUrl();
	}

	return FString();
}

void UXsollaPayStationBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	PayStationBrowserWidget.Reset();
}

TSharedRef<SWidget> UXsollaPayStationBrowser::RebuildWidget()
{
	// clang-format off
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Xsolla PayStation Browser", "Xsolla PayStation Browser"))
			];
	}
	else
	{
		PayStationBrowserWidget = SNew(SWebBrowser)
			.InitialURL(InitialURL)
			.ShowControls(false)
			.SupportsTransparency(bSupportsTransparency)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup));

		return PayStationBrowserWidget.ToSharedRef();
	}
	// clang-format on
}

void UXsollaPayStationBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

bool UXsollaPayStationBrowser::HandleOnBeforePopup(FString URL, FString Frame)
{
	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			// clang-format off
			// Retry on the GameThread now
			TWeakObjectPtr<UXsollaPayStationBrowser> WeakThis = this;
			FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->HandleOnBeforePopup(URL, Frame);
				}
			}, TStatId(), nullptr, ENamedThreads::GameThread);
			// clang-format on
		}

		return true;
	}

	return false;
}

#if WITH_EDITOR
const FText UXsollaPayStationBrowser::GetPaletteCategory()
{
	return LOCTEXT("Xsolla", "Xsolla");
}
#endif

#undef LOCTEXT_NAMESPACE
