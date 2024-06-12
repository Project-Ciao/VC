// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TileSystemComponent.h"
#include "TileSystemEditorModeSettings.generated.h"

/**
 * 
 */
UCLASS()
class TILESYSTEMEDITOR_API UTileSystemEditorModeSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Editor Mode Settings")
		FIntVector DragLimit = FIntVector(32, 32, 0);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Editor Mode Settings")
		int32 ZLevel = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Editor Mode Settings", meta = (Bitmask))
		uint8 BitMask = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Editor Mode Settings")
		ETileSystemOperation LeftClickOperation = ETileSystemOperation::Add;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Editor Mode Settings")
		ETileSystemOperation CtrlLeftClickOperation = ETileSystemOperation::Remove;
};
