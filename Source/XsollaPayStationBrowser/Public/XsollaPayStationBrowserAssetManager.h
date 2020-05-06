// Copyright 2020 Xsolla Inc. All Rights Reserved.

#pragma once

#include "Materials/Material.h"
#include "UObject/SoftObjectPtr.h"

#include "XsollaPayStationBrowserAssetManager.generated.h"

class UMaterial;

UCLASS()
class XSOLLAPAYSTATIONBROWSER_API UXsollaPayStationBrowserAssetManager : public UObject
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
