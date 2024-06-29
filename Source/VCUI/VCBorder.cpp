// Fill out your copyright notice in the Description page of Project Settings.


#include "VCBorder.h"
#include "Styles/VCDefaultBorderStyle.h"

UVCBorder::UVCBorder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Padding = FMargin(48.0, 64.0);
	SetStyle(UVCDefaultBorderStyle::StaticClass());
}

#if WITH_EDITOR
const FText UVCBorder::GetPaletteCategory()
{
	return INVTEXT("VC");
}
#endif
