// Fill out your copyright notice in the Description page of Project Settings.


#include "VCBorder.h"
#include "Styles/VCDefaultBorderStyle.h"

static const FName OnStyleChangeFunctionName("OnStyleChange");

UVCBorder::UVCBorder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetPadding(FMargin(48.0, 64.0));
	SetStyle(UVCDefaultBorderStyle::StaticClass());

	UVCDefaultBorderStyle::StaticClass()->GetDefaultObject<UVCDefaultBorderStyle>()->OnStyleChange.AddUFunction(this, OnStyleChangeFunctionName);
}

void UVCBorder::OnStyleChange()
{
	if (MyBorder.IsValid())
	{
		// Don't synchronize properties if there is no slate widget.
		SynchronizeProperties();
	}
}

#if WITH_EDITOR
const FText UVCBorder::GetPaletteCategory()
{
	return INVTEXT("VC");
}
#endif
