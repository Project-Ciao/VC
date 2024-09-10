// Fill out your copyright notice in the Description page of Project Settings.

#include "VCEditor.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#include <cmath>

#define LOCTEXT_NAMESPACE "FVCEditor"

#include "ISettingsModule.h"
#include "ISettingsSection.h"

#include "LevelEditor.h"
#include "ToolMenuEntry.h"
#include "ToolMenus.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SSpinBox.h"

#include "Kismet/GameplayStatics.h"

#include "TimeOfDay/TimeOfDayControllerInterface.h"

UE_DISABLE_OPTIMIZATION

static float TimeOfDayPlayInEditorRate = 1.f;
static bool bTimeOfDayPlayInEditor = false;

static AActor* GetTimeOfDayController()
{
	// First look for a PIE world to grab the time of day controller
	UWorld* World = nullptr;
	if (FWorldContext* PIEWorld = GEditor->GetPIEWorldContext(0))
	{
		World = PIEWorld->World();
	}

	if (World == nullptr)
	{
		World = GWorld;
	}

	if (World != nullptr)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(World, UTimeOfDayControllerInterface::StaticClass(), Actors);
		if (Actors.Num() > 0)
		{
			return Actors[0];
		}
	}
	return nullptr;
}

class FVCEditorCommands : public TCommands<FVCEditorCommands>
{
public:
	FVCEditorCommands() 
		: TCommands<FVCEditorCommands>("VCEditor", LOCTEXT("Editor", "Editor"), NAME_None, FAppStyle::GetAppStyleSetName())
	{
	}

	// TCommand<> interface
	virtual void RegisterCommands() override
	{
		// Show toggles
		UI_COMMAND(PlayTimeOfDayCommand, "Play Time Of Day", "Makes the time of day system play in the editor world.", EUserInterfaceActionType::ToggleButton, FInputChord());
	}
	// End of TCommand<> interface

	static void BindCommandsToActions()
	{
		static bool bInitializedGlobalPlayWorldCommands = false;
		check(!bInitializedGlobalPlayWorldCommands);
		bInitializedGlobalPlayWorldCommands = true;

		const FVCEditorCommands& Commands = Get();
		FUICommandList& ActionList = *Actions;

		ActionList.MapAction(Commands.PlayTimeOfDayCommand,
			FExecuteAction::CreateLambda([]()
			{
				bTimeOfDayPlayInEditor = !bTimeOfDayPlayInEditor;
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([]()
			{
				return bTimeOfDayPlayInEditor;
			})
		);
	}

	TSharedPtr<FUICommandInfo> PlayTimeOfDayCommand;

	static TSharedPtr<FUICommandList> Actions;
};

TSharedPtr<FUICommandList> FVCEditorCommands::Actions(new FUICommandList());

static TSharedRef<SWidget> MakeTimeOfDaySlider()
{
	return SNew(SBox)
	.MinDesiredWidth(220)
	[
		SNew(SSlider)
		.Value_Lambda([]() -> float
		{
			if (AActor* TimeOfDayController = GetTimeOfDayController())
			{
				return FMath::Frac(ITimeOfDayControllerInterface::Execute_GetTimeOfDay(TimeOfDayController) / 360.f);
			}
			return 0.f;
		})
		.IsEnabled_Lambda([]() -> bool
		{
			return GetTimeOfDayController() != nullptr;
		})
		.OnValueChanged_Lambda([](float NewValue)
		{
			if (AActor* TimeOfDayController = GetTimeOfDayController())
			{
				ITimeOfDayControllerInterface::Execute_SetTimeOfDay(TimeOfDayController, NewValue * 360.f * (std::nextafter(1.0f, 0.0f)));
			}
		})
		.ToolTipText_Lambda([]() -> FText
		{
			if (AActor* TimeOfDayController = GetTimeOfDayController())
			{
				return ITimeOfDayControllerInterface::Execute_GetTimeOfDayText(TimeOfDayController);
			}
			return FText::GetEmpty();
		})
	];
}

static TSharedRef<SWidget> MakeTimeOfDayPlayRateWidget()
{
	return SNew(SBox)
	.Padding(4, 0)
	[
		SNew(SSpinBox<float>)
		.MinSliderValue(-100.f)
		.MaxSliderValue(100.f)
		.MinFractionalDigits(1)
		.MaxFractionalDigits(3)
		.MinDesiredWidth(70.f)
		.SliderExponent(2.f)
		.SliderExponentNeutralValue(0.f)
		.Delta(1.f / 128.f)
		.ToolTipText(LOCTEXT("PlayTimeOfDayRateEntryToolTip", "How fast the time of day should play in editor."))
		.Value_Lambda([]() { return TimeOfDayPlayInEditorRate; })
		.OnValueCommitted_Lambda([](float NewValue, ETextCommit::Type)
		{
			TimeOfDayPlayInEditorRate = NewValue;
		})
		.OnValueChanged_Lambda([](float NewValue)
		{
			TimeOfDayPlayInEditorRate = NewValue;
		})
	];
}

void FVCEditor::StartupModule()
{
	TimeOfDayPlayInEditorRate = 1.f;
	bTimeOfDayPlayInEditor = false;

	FVCEditorCommands::Register();
	FVCEditorCommands::BindCommandsToActions();

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FVCEditor::RegisterMenus));

	TickDelegate = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FVCEditor::Tick));
}

void FVCEditor::ShutdownModule()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegate);
}

void FVCEditor::RegisterMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	FToolMenuSection& Section = Menu->FindOrAddSection("VCEditor");

	TSharedRef<SWidget> VolumeItem = SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.AutoWidth()
	.Padding(4, 0)
	.VAlign(VAlign_Center)
	[
		SNew(SImage)
		.Image(FAppStyle::GetBrush(TEXT("GraphEditor.TimelineGlyph")))
		.ToolTipText(LOCTEXT("TimeOfDayLabel", "Time Of Day"))
	]
	+ SHorizontalBox::Slot().AutoWidth()
	[
		MakeTimeOfDaySlider()
	]
	+ SHorizontalBox::Slot()
	.FillWidth(0.1f);

	Section.AddSeparator("VCEditorSpacer");
	Section.AddEntry(FToolMenuEntry::InitWidget("TimeOfDay", VolumeItem, LOCTEXT("TimeOfDayLabel", "Time Of Day")));

	FToolMenuEntry PlayTimeOfDayMenuEntry = 
		FToolMenuEntry::InitToolBarButton(
			FVCEditorCommands::Get().PlayTimeOfDayCommand,
			TAttribute<FText>(),
			TAttribute<FText>(),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Toolbar.Play"),
			FName(TEXT("PlayTimeOfDayCommand"))
		);
	PlayTimeOfDayMenuEntry.StyleNameOverride = FName("Toolbar.BackplateLeftPlay");
	PlayTimeOfDayMenuEntry.SetCommandList(FVCEditorCommands::Actions);

	FToolMenuEntry PlayTimeOfDayRateEntry = FToolMenuEntry::InitWidget("PlayTimeOfDayRate", MakeTimeOfDayPlayRateWidget(), LOCTEXT("PlayTimeOfDayRateWidget", "Time Of Day Play Rate"));
	PlayTimeOfDayRateEntry.StyleNameOverride = FName("Toolbar.BackplateRight");

	// Play
	Section.AddEntry(PlayTimeOfDayMenuEntry);
	Section.AddEntry(PlayTimeOfDayRateEntry);
}

bool FVCEditor::Tick(float DeltaTime)
{
	if (bTimeOfDayPlayInEditor)
	{
		if (AActor* TimeOfDayController = GetTimeOfDayController())
		{
			// Don't tick it in PIE
			if (FWorldContext* PIEWorld = GEditor->GetPIEWorldContext(0))
			{
				return true;
			}

			DeltaTime *= TimeOfDayPlayInEditorRate;
			const float CurrentTime = ITimeOfDayControllerInterface::Execute_GetTimeOfDay(TimeOfDayController);
			ITimeOfDayControllerInterface::Execute_SetTimeOfDay(TimeOfDayController, CurrentTime + DeltaTime);
		}
	}
	return true;
}

UE_ENABLE_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVCEditor, VCEditor);
