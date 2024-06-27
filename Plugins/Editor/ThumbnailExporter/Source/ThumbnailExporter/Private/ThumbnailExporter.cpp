// Copyright 2023 Big Cat Energising. All Rights Reserved.


#include "ThumbnailExporter.h"

#include "ContentBrowserModule.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "IContentBrowserSingleton.h"
#include "ThumbnailExporterSettings.h"
#include "ThumbnailExporterRenderer.h"
#include "BlueprintThumbnailExporterRenderer.h"
#include "ThumbnailExporterThumbnailDummy.h"

#define LOCTEXT_NAMESPACE "FThumbnailExporterModule"
void FThumbnailExporterModule::StartupModule()
{
	UThumbnailManager::Get().RegisterCustomRenderer(UThumbnailExporterThumbnailDummy::StaticClass(), UBlueprintThumbnailExporterRenderer::StaticClass());

	AddContentBrowserContextMenuExtender();
}

void FThumbnailExporterModule::ShutdownModule()
{
	RemoveContentBrowserContextMenuExtender();
}

void FThumbnailExporterModule::AddContentBrowserContextMenuExtender()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetExtenderDelegates = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();

	CBMenuAssetExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&OnExtendContentBrowserAssetSelectionMenu));
	ContentBrowserExtenderDelegateHandle = CBMenuAssetExtenderDelegates.Last().GetHandle();
}

void FThumbnailExporterModule::RemoveContentBrowserContextMenuExtender() const
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	CBMenuExtenderDelegates.RemoveAll([this](const FContentBrowserMenuExtender_SelectedAssets& Delegate) { 
		return Delegate.GetHandle() == ContentBrowserExtenderDelegateHandle;
	});
}

TSharedRef<FExtender> FThumbnailExporterModule::OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();
	Extender->AddMenuExtension(
		"CommonAssetActions",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateStatic(&ExecuteSaveThumbnailAsTexture, SelectedAssets)
	);
	return Extender;
}

bool FThumbnailExporterModule::CanCreateThumbnail(const TArray<FAssetData>& SelectedAssets)
{
	UBlueprintThumbnailExporterRenderer* ThumbnailRenderer = UBlueprintThumbnailExporterRenderer::StaticClass()->GetDefaultObject<UBlueprintThumbnailExporterRenderer>();
	if (ThumbnailRenderer == nullptr)
	{
		return false;
	}

	for (const FAssetData& AssetData : SelectedAssets)
	{
		if (AssetData.IsValid())
		{
			if (ThumbnailRenderer->CanVisualizeAsset(AssetData.GetAsset()))
			{
				return true;
			}
		}
	}

	return false;
}

void FThumbnailExporterModule::ExecuteSaveThumbnailAsTexture(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets)
{
	// Only create the menu if a blueprint is selected and it's renderable
	if (!CanCreateThumbnail(SelectedAssets))
	{
		return;
	}

	const TArray<FThumbnailCreationPreset>& ThumbnailCreationPresets = UThumbnailExporterSettings::Get()->ThumbnailCreationPresets;

	if (ThumbnailCreationPresets.Num() == 1)
	{
		MenuBuilder.BeginSection("Thumbnail Exporter", LOCTEXT("ThumbnailExporterAssetContext", "Thumbnail Exporter"));
		{
			MenuBuilder.AddMenuEntry(
				ThumbnailCreationPresets[0].MenuItemName,
				ThumbnailCreationPresets[0].MenuItemTooltip,
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([SelectedAssets]()
				{
					for (const FAssetData& Asset : SelectedAssets)
					{
						if (CanCreateThumbnail({ Asset }))
						{
							FString OutFilePath;
							ExportThumbnail(UThumbnailExporterSettings::Get()->ThumbnailCreationPresets[0].PresetConfig, Asset, OutFilePath);
						}
					}
				})),
				NAME_None,
				EUserInterfaceActionType::Button
			);
		}
		MenuBuilder.EndSection();
	}
	else if (ThumbnailCreationPresets.Num() > 1)
	{
		MenuBuilder.BeginSection("Thumbnail Exporter", LOCTEXT("ThumbnailExporterAssetContext", "Thumbnail Exporter"));
		{
			MenuBuilder.AddSubMenu(
				LOCTEXT("ThumbnailExporterPresets", "Thumbnail Export Presets"),
				LOCTEXT("ThumbnailExporterPresetsTooltip", "Menu for the export preset list. The export presets are defined in the Thumbnail Exporter config in the project settings."),
				FNewMenuDelegate::CreateLambda([SelectedAssets](FMenuBuilder& InSubMenuBuilder)
				{
					const TArray<FThumbnailCreationPreset>& ThumbnailCreationPresets = UThumbnailExporterSettings::Get()->ThumbnailCreationPresets;
					for (int32 i = 0; i < ThumbnailCreationPresets.Num(); ++i)
					{
						InSubMenuBuilder.AddMenuEntry(
							ThumbnailCreationPresets[i].MenuItemName,
							ThumbnailCreationPresets[i].MenuItemTooltip,
							FSlateIcon(),
							FUIAction(FExecuteAction::CreateLambda([SelectedAssets, i]()
							{
								for (const FAssetData& Asset : SelectedAssets)
								{
									if (CanCreateThumbnail({ Asset }))
									{
										FString OutFilePath;
										ExportThumbnail(UThumbnailExporterSettings::Get()->ThumbnailCreationPresets[i].PresetConfig, Asset, OutFilePath);
									}
								}
							})),
							NAME_None,
							EUserInterfaceActionType::Button
						);
					}
				}),
				false,
				FSlateIcon()
			);
		}
		MenuBuilder.EndSection();
	}
}

bool FThumbnailExporterModule::GetThumbnailAssetPathAndFilename(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& Path, FString& Filename)
{
	const FString AssetPath = Asset.GetAsset()->GetPathName();

	FString FullPath;
	if (CreationConfig.bOverrideThumbnailFilename)
	{
		FullPath = CreationConfig.ThumbnailOverrideFilename;
	}
	else
	{
		const FString& Prefix = CreationConfig.ThumbnailPrefix;
		const FString& Suffix = CreationConfig.ThumbnailSuffix;

		FullPath = Prefix + FPaths::GetBaseFilename(AssetPath) + Suffix;
	}

	if (CreationConfig.bOverrideThumbnailPath)
	{
		FullPath = CreationConfig.ThumbnailOverridePath.Path / FullPath;
	}
	else
	{
		FullPath = FPaths::GetPath(AssetPath) / FullPath;
	}

	Path = FPaths::GetPath(FullPath);
	Filename = FPaths::GetBaseFilename(FullPath);

	return true;
}

static UPackage* GetAssetPackage(const FThumbnailCreationConfig& CreationConfig, const FString& FullPath)
{
	UPackage* Package = CreatePackage(*FullPath);
	if (Package != nullptr)
	{
		Package->FullyLoad();
	}

	return Package;
}

bool FThumbnailExporterModule::ExportThumbnail(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& ThumbnailPath, const FPreCreateThumbnail& CreationDelegate)
{
	FThumbnailCreationConfig ModifiedCreationConfig = CreationConfig;

	FString AssetPath, AssetFilename;
	if (!GetThumbnailAssetPathAndFilename(ModifiedCreationConfig, Asset, AssetPath, AssetFilename))
	{
		return false;
	}
	ThumbnailPath = AssetPath / AssetFilename;

	UPackage* Package = GetAssetPackage(ModifiedCreationConfig, ThumbnailPath);
	if (Package == nullptr)
	{
		return false;
	}

	FObjectThumbnail* Thumb = FThumbnailExporterRenderer::GenerateThumbnail(ModifiedCreationConfig, Asset.GetAsset(), CreationDelegate);
	if (!Thumb)
	{
		return false;
	}

	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, *AssetFilename, RF_Public | RF_Standalone);
	NewTexture->AddToRoot();
	FTexturePlatformData* PlatformData = new FTexturePlatformData();
	PlatformData->SizeX = Thumb->GetImageWidth();
	PlatformData->SizeY = Thumb->GetImageHeight();
	PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;
	NewTexture->SetPlatformData(PlatformData);

	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	PlatformData->Mips.Add(Mip);
	Mip->SizeX = Thumb->GetImageWidth();
	Mip->SizeY = Thumb->GetImageHeight();

	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(Thumb->GetUncompressedImageData().Num());
	FMemory::Memcpy(TextureData, Thumb->GetUncompressedImageData().GetData(), Thumb->GetUncompressedImageData().Num());
	Mip->BulkData.Unlock();

	NewTexture->Source.Init(Thumb->GetImageWidth(), Thumb->GetImageHeight(), 1, 1, ETextureSourceFormat::TSF_BGRA8, Thumb->GetUncompressedImageData().GetData());
	NewTexture->LODGroup = ModifiedCreationConfig.ThumbnailTextureGroup;
	NewTexture->UpdateResource();
	Package->MarkPackageDirty();
	Package->FullyLoad();
	FAssetRegistryModule::AssetCreated(NewTexture);

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;
	SaveArgs.bForceByteSwapping = true;
	FString PackageFileName = FPackageName::LongPackageNameToFilename(ThumbnailPath, FPackageName::GetAssetPackageExtension());
	UPackage::SavePackage(Package, NewTexture, *PackageFileName, SaveArgs);

	if (ModifiedCreationConfig.bCreateThumbnailNotification)
	{
		CreateThumbnailNotification(NewTexture);
	}

	return true;
}

void FThumbnailExporterModule::CreateThumbnailNotification(UTexture2D* NewTexture)
{
	FNotificationInfo NotificationInfo(LOCTEXT("GeneratedAssetIconNotification", "Generated asset icon"));
	NotificationInfo.ExpireDuration = 5.0f;

	const FSoftObjectPath SoftObjectPath(NewTexture);
	NotificationInfo.Hyperlink = FSimpleDelegate::CreateLambda([SoftObjectPath] {
		// Select the texture in Content Browser when the hyperlink is clicked
#if ENGINE_MINOR_VERSION == 0
		FAssetData AssetData = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get().GetAssetByObjectPath(SoftObjectPath.GetAssetPathName());
#else
		FAssetData AssetData = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get().GetAssetByObjectPath(SoftObjectPath.GetWithoutSubPath());
#endif
		FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get().SyncBrowserToAssets({ AssetData });
	});

	NotificationInfo.HyperlinkText = FText::FromString(SoftObjectPath.ToString());
	FSlateNotificationManager::Get().AddNotification(NotificationInfo);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FThumbnailExporterModule, ThumbnailExporter)