// Copyright 2020 Xsolla Inc. All Rights Reserved.

#include "XsollaPaymentsBrowser.h"

#include "Async/TaskGraphInterfaces.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "XsollaPaymentsBrowser"

UXsollaPaymentsBrowser::UXsollaPaymentsBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UXsollaPaymentsBrowser::LoadURL(FString NewURL)
{
	if (PaymentsBrowserWidget.IsValid())
	{
		return PaymentsBrowserWidget->LoadURL(NewURL);
	}
}

FString UXsollaPaymentsBrowser::GetUrl() const
{
	if (PaymentsBrowserWidget.IsValid())
	{
		return PaymentsBrowserWidget->GetUrl();
	}

	return FString();
}

void UXsollaPaymentsBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	PaymentsBrowserWidget.Reset();
}

TSharedRef<SWidget> UXsollaPaymentsBrowser::RebuildWidget()
{
	// clang-format off
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Xsolla Payments Browser", "Xsolla Payments Browser"))
			];
	}
	else
	{
		PaymentsBrowserWidget = SNew(SWebBrowser)
			.InitialURL(InitialURL)
			.ShowControls(false)
			.SupportsTransparency(bSupportsTransparency)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup));

		return PaymentsBrowserWidget.ToSharedRef();
	}
	// clang-format on
}

void UXsollaPaymentsBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

bool UXsollaPaymentsBrowser::HandleOnBeforePopup(FString URL, FString Frame)
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
			TWeakObjectPtr<UXsollaPaymentsBrowser> WeakThis = this;
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
const FText UXsollaPaymentsBrowser::GetPaletteCategory()
{
	return LOCTEXT("Xsolla", "Xsolla");
}
#endif

#undef LOCTEXT_NAMESPACE
