// Copyright 2023 Big Cat Energising. All Rights Reserved.


#include "ThumbnailExporterRenderer.h"
#include "ThumbnailExporterSettings.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "TextureCompiler.h"
#include "Misc/ScopedSlowTask.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "ThumbnailRendering/TextureThumbnailRenderer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasTypes.h"
#include "ShaderCompiler.h"
#include "ContentStreaming.h"
#include "ThumbnailExporterThumbnailDummy.h"
#include "BlueprintThumbnailExporterRenderer.h"

#if ENGINE_MINOR_VERSION == 0
static void TransitionAndCopyTexture(FRHICommandList& RHICmdList, FRHITexture* Source, FRHITexture* Destination, const FRHICopyTextureInfo& CopyInfo)
{
	FRHITransitionInfo TransitionsBefore[] = {
		FRHITransitionInfo(Source, ERHIAccess::SRVMask, ERHIAccess::CopySrc),
		FRHITransitionInfo(Destination, ERHIAccess::SRVMask, ERHIAccess::CopyDest)
	};

	RHICmdList.Transition(MakeArrayView(TransitionsBefore, UE_ARRAY_COUNT(TransitionsBefore)));

	RHICmdList.CopyTexture(Source, Destination, CopyInfo);

	FRHITransitionInfo TransitionsAfter[] = {
		FRHITransitionInfo(Source, ERHIAccess::CopySrc, ERHIAccess::SRVMask),
		FRHITransitionInfo(Destination, ERHIAccess::CopyDest, ERHIAccess::SRVMask)
	};

	RHICmdList.Transition(MakeArrayView(TransitionsAfter, UE_ARRAY_COUNT(TransitionsAfter)));
}
#endif

FObjectThumbnail* FThumbnailExporterRenderer::GenerateThumbnail(FThumbnailCreationConfig& CreationConfig, UObject* InObject, const FPreCreateThumbnail& CreationDelegate)
{
	// Does the object support thumbnails?
	FThumbnailRenderingInfo* RenderInfo = GUnrealEd ? GUnrealEd->GetThumbnailManager()->GetRenderingInfo(UThumbnailExporterThumbnailDummy::StaticClass()->ClassDefaultObject) : nullptr;
	if (RenderInfo != NULL && RenderInfo->Renderer != nullptr)
	{
		// Set the size of cached thumbnails
		const int32 ImageWidth = CreationConfig.ThumbnailSize;
		const int32 ImageHeight = CreationConfig.ThumbnailSize;

		// For cached thumbnails we want to make sure that textures are fully streamed in so that the thumbnail we're saving won't have artifacts
		// However, this can add 30s - 100s to editor load
		//@todo - come up with a cleaner solution for this, preferably not blocking on texture streaming at all but updating when textures are fully streamed in
		ThumbnailTools::EThumbnailTextureFlushMode::Type TextureFlushMode = ThumbnailTools::EThumbnailTextureFlushMode::AlwaysFlush;

		// Generate the thumbnail
		FObjectThumbnail NewThumbnail;
		FThumbnailExporterRenderer::RenderThumbnail(
			CreationConfig, InObject, ImageWidth, ImageHeight, TextureFlushMode,
			&NewThumbnail, CreationDelegate);

		UPackage* MyOutermostPackage = InObject->GetOutermost();
		return ThumbnailTools::CacheThumbnail(InObject->GetFullName(), &NewThumbnail, MyOutermostPackage);
	}

	return NULL;
}

struct FThumbnailRenderTargetResource
{
	UTextureRenderTarget2D* RenderTargetTexture;
	FTextureRenderTargetResource* RenderTargetResource;
	FCanvas Canvas;
};

static FThumbnailRenderTargetResource CreateThumbnailRenderTarget(uint32 InImageWidth, uint32 InImageHeight, FLinearColor ClearColor)
{
	const uint32 MinRenderTargetSize = FMath::Max(InImageWidth, InImageHeight);
	UTextureRenderTarget2D* RenderTargetTexture = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
	check(RenderTargetTexture != NULL);

	RenderTargetTexture->TargetGamma = GEngine->DisplayGamma;
	RenderTargetTexture->RenderTargetFormat = RTF_RGBA8;
	RenderTargetTexture->ClearColor = ClearColor;
	RenderTargetTexture->InitAutoFormat(InImageWidth, InImageHeight);
	RenderTargetTexture->UpdateResourceImmediate(true);

	// Make sure the input dimensions are OK.  The requested dimensions must be less than or equal to
	// our scratch render target size.
	check(InImageWidth <= RenderTargetTexture->GetSurfaceWidth());
	check(InImageHeight <= RenderTargetTexture->GetSurfaceHeight());

	FTextureRenderTargetResource* RenderTargetResource = RenderTargetTexture->GameThread_GetRenderTargetResource();
	check(RenderTargetResource != NULL);

	// Create a canvas for the render target and clear it
	FCanvas Canvas(RenderTargetResource, NULL, FGameTime::GetTimeSinceAppStart(), GMaxRHIFeatureLevel);
	Canvas.Clear(ClearColor);

	return FThumbnailRenderTargetResource{ RenderTargetTexture, RenderTargetResource, MoveTemp(Canvas)};
}

void FThumbnailExporterRenderer::RenderThumbnail(FThumbnailCreationConfig& CreationConfig, UObject* InObject, 
	const uint32 InImageWidth, const uint32 InImageHeight, ThumbnailTools::EThumbnailTextureFlushMode::Type InFlushMode, FObjectThumbnail* OutThumbnail, const FPreCreateThumbnail& CreationDelegate)
{
	if (!FApp::CanEverRender())
	{
		return;
	}

	TRACE_CPUPROFILER_EVENT_SCOPE(FThumbnailExporterRenderer::RenderThumbnail);

	// Renderer must be initialized before generating thumbnails
	check(GIsRHIInitialized);

	// Store dimensions
	if (OutThumbnail)
	{
		OutThumbnail->SetImageSize(InImageWidth, InImageHeight);
	}

	FThumbnailRenderTargetResource LDRThumbnail = CreateThumbnailRenderTarget(InImageWidth, InImageHeight, CreationConfig.GetAdjustedBackgroundColor());
	FThumbnailRenderTargetResource AlphaThumbnail = CreateThumbnailRenderTarget(InImageWidth, InImageHeight, CreationConfig.GetAdjustedBackgroundColor());

	// Get the rendering info for this object
	FThumbnailRenderingInfo* RenderInfo = GUnrealEd ? GUnrealEd->GetThumbnailManager()->GetRenderingInfo(UThumbnailExporterThumbnailDummy::StaticClass()->ClassDefaultObject) : nullptr;

	// Wait for all textures to be streamed in before we render the thumbnail
	// @todo CB: This helps but doesn't result in 100%-streamed-in resources every time! :(
	if (InFlushMode == ThumbnailTools::EThumbnailTextureFlushMode::AlwaysFlush)
	{
		if (GShaderCompilingManager)
		{
			GShaderCompilingManager->ProcessAsyncResults(false, true);
		}

		if (UTexture* Texture = Cast<UTexture>(InObject))
		{
			FTextureCompilingManager::Get().FinishCompilation({ Texture });
		}

		FlushAsyncLoading();

		IStreamingManager::Get().StreamAllResources(100.0f);
	}

	if (RenderInfo != NULL && RenderInfo->Renderer != NULL)
	{
		// Make sure we suppress any message dialogs that might result from constructing
		// or initializing any of the renderable objects.
		TGuardValue<bool> Unattended(GIsRunningUnattendedScript, true);

		// Draw the thumbnail
		const bool bAdditionalViewFamily = false;

		UBlueprintThumbnailExporterRenderer* OurThumbnailRenderer = Cast<UBlueprintThumbnailExporterRenderer>(RenderInfo->Renderer);
		check(OurThumbnailRenderer != nullptr);


		{
			// Draw the LDR/final color thumbnail
			FThumbnailCreationParams CreationParams(CreationConfig);
			CreationParams.Object = InObject;
			CreationParams.Width = InImageWidth;
			CreationParams.Height = InImageHeight;
			CreationParams.bIsAlpha = false;
			CreationParams.RenderTarget = LDRThumbnail.RenderTargetResource;
			CreationParams.Canvas = &LDRThumbnail.Canvas;
			CreationParams.bAdditionalViewFamily = bAdditionalViewFamily;
			CreationParams.CreationDelegate = CreationDelegate;

			OurThumbnailRenderer->DrawThumbnailWithConfig(CreationParams);
		}

		{
			// Draw the alpha
			FThumbnailCreationParams CreationParams(CreationConfig);
			CreationParams.Object = InObject;
			CreationParams.Width = InImageWidth;
			CreationParams.Height = InImageHeight;
			CreationParams.bIsAlpha = true;
			CreationParams.RenderTarget = AlphaThumbnail.RenderTargetResource;
			CreationParams.Canvas = &AlphaThumbnail.Canvas;
			CreationParams.bAdditionalViewFamily = bAdditionalViewFamily;
			CreationParams.CreationDelegate = CreationDelegate;

			OurThumbnailRenderer->DrawThumbnailWithConfig(CreationParams);
		}
	}

	// Tell the rendering thread to draw any remaining batched elements
	LDRThumbnail.Canvas.Flush_GameThread();
	AlphaThumbnail.Canvas.Flush_GameThread();

	ENQUEUE_RENDER_COMMAND(UpdateThumbnailRTCommand)(
		[LDRRenderTargetResource = LDRThumbnail.RenderTargetResource, AlphaRenderTargetResource = AlphaThumbnail.RenderTargetResource](FRHICommandListImmediate& RHICmdList)
		{
			TransitionAndCopyTexture(RHICmdList, LDRRenderTargetResource->GetRenderTargetTexture(), LDRRenderTargetResource->TextureRHI, {});
			TransitionAndCopyTexture(RHICmdList, AlphaRenderTargetResource->GetRenderTargetTexture(), AlphaRenderTargetResource->TextureRHI, {});
		}
	);

	FlushRenderingCommands();

	if (OutThumbnail)
	{
		const FIntRect InSrcRect(0, 0, OutThumbnail->GetImageWidth(), OutThumbnail->GetImageHeight());

		TArray<uint8>& OutData = OutThumbnail->AccessImageData();

		OutData.Empty();
		OutData.AddUninitialized(OutThumbnail->GetImageWidth() * OutThumbnail->GetImageHeight() * sizeof(FColor));

		// Copy the contents of the LDR color to the thumbnail
		// NOTE: OutData must be a preallocated buffer!
		LDRThumbnail.RenderTargetResource->ReadPixelsPtr((FColor*)OutData.GetData(), FReadSurfaceDataFlags(), InSrcRect);

		TArray<uint8> AlphaData;
		AlphaData.AddUninitialized(OutThumbnail->GetImageWidth() * OutThumbnail->GetImageHeight() * sizeof(FColor));

		AlphaThumbnail.RenderTargetResource->ReadPixelsPtr((FColor*)AlphaData.GetData(), FReadSurfaceDataFlags(), InSrcRect);

		FColor* Color = (FColor*)OutData.GetData();
		FColor* Alpha = (FColor*)AlphaData.GetData();
		if (CreationConfig.InvertBackgroundAlpha())
		{
			for (; Color < (FColor*)(OutData.GetData() + OutData.Num()); ++Color, ++Alpha)
			{
				Color->A = 255 - Alpha->A;
			}
		}
		else
		{
			for (; Color < (FColor*)(OutData.GetData() + OutData.Num()); ++Color, ++Alpha)
			{
				Color->A = Alpha->A;
			}
		}
	}
}