// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Materials/Material.h"
#include "UObject/SoftObjectPtr.h"

#include "XsollaPaymentsBrowserAssetManager.generated.h"

class UMaterial;

UCLASS()
class XSOLLAPAYMENTSBROWSER_API UXsollaPaymentsBrowserAssetManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	void LoadDefaultMaterials();

	UMaterial* GetDefaultMaterial() const;
	UMaterial* GetDefaultTranslucentMaterial() const;

protected:
	UPROPERTY()
	TSoftObjectPtr<UMaterial> DefaultMaterial;

	UPROPERTY()
	TSoftObjectPtr<UMaterial> DefaultTranslucentMaterial;
};
