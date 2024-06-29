// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonBorder.h"
#include "VCBorder.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = UI, meta = (Category = "VC", DisplayName = "VC Border"))
class VCUI_API UVCBorder : public UCommonBorder
{
	GENERATED_BODY()
	
public:
	UVCBorder(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	//virtual void OnCreationFromPalette() override;
	const FText GetPaletteCategory() override;
	//virtual void OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs) override;
#endif // WITH_EDITOR
};
