// Copyright Epic Games, Inc. All Rights Reserved.

#include "VC.h"
#include "Modules/ModuleManager.h"
#include "CanvasTypes.h"
#include "VCPlayerController.h"

DEFINE_LOG_CATEGORY(LogVC);

#define LOCTEXT_NAMESPACE "FVC"

class FVCModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		if (GEngine == nullptr)
		{
			FCoreDelegates::OnPostEngineInit.AddLambda([this]()
			{
				AddVCLevelStat();
			});
		}
		else
		{
			AddVCLevelStat();
		}
	}

	void AddVCLevelStat()
	{
		check(GEngine != nullptr);

		GEngine->AddEngineStat(TEXT("STAT_VCLevels"), TEXT("STATCAT_VC"), LOCTEXT("VCLevelStatDesc", "Displays stats on what levels are currently in the PC's VisibleLevels/InvisibleLevels sets."),
			UEngine::FEngineStatRender::CreateRaw(this, &FVCModule::RenderLevelStats), UEngine::FEngineStatToggle::CreateRaw(this, &FVCModule::ToggleLevelStats), false);
	}

	int32 RenderLevelStats(UWorld* World, FViewport* Viewport, FCanvas* Canvas, int32 X, int32 Y, const FVector* ViewLocation, const FRotator* ViewRotation)
	{
		check(IsInGameThread());

		int32 MaxY = Y;
		
		Canvas->DrawShadowedString(X, Y, TEXT("VC Levels"), GEngine->GetSmallFont(), FLinearColor::White);
		Y += 12;

		// Render invisible levels in red, and visible ones in green.
		if (const AVCPlayerController* PC = World->GetFirstPlayerController<AVCPlayerController>())
		{
			Canvas->DrawShadowedString(X + 4, Y, TEXT("  Visible:"), GEngine->GetSmallFont(), FLinearColor::White);
			Y += 12;

			for (FName VisibleLevel : PC->VisibleLevels)
			{
				Canvas->DrawShadowedString(X + 4, Y, *("    " + VisibleLevel.ToString()), GEngine->GetSmallFont(), FLinearColor::Green);
				Y += 12;
			}

			Canvas->DrawShadowedString(X + 4, Y, TEXT("  Invisible:"), GEngine->GetSmallFont(), FLinearColor::White);
			Y += 12;

			for (FName InvisibleLevel : PC->InvisibleLevels)
			{
				Canvas->DrawShadowedString(X + 4, Y, *("    " + InvisibleLevel.ToString()), GEngine->GetSmallFont(), FLinearColor::Red);
				Y += 12;
			}
		}

		return FMath::Max(MaxY, Y);
	}

	bool ToggleLevelStats(UWorld* World, FCommonViewportClient* ViewportClient, const TCHAR* Stream)
	{
		CollectingLevelStats = !CollectingLevelStats;
		return false;
	}

protected:
	TAtomic<bool> CollectingLevelStats = false;
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_PRIMARY_GAME_MODULE( FVCModule, VC, "VC" );
