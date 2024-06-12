// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSystemEdMode.h"

#include "TileDebugDrawComponent.h"
#include "TileCursorDebugDrawComponent.h"
#include "TileSystemComponent.h"
#include "EditorViewportClient.h"
#include "Engine/Selection.h"
#include "EditorModeManager.h"
#include "Toolkits/ToolkitManager.h"
#include "Blueprint/UserWidget.h"
#include "TileSystemEditorSettings.h"
#include "TileSystemToolkitWidgetInterface.h"
#include "TileSystemEditorModeSettings.h"

FTileSystemEdMode::FTileSystemEdMode()
	: FEdMode()
{
	TileDrawComponent = NewObject<UTileDebugDrawComponent>(GetTransientPackage(), TEXT("TileDrawComponent"));
	TileCursorDrawComponent = NewObject<UTileCursorDebugDrawComponent>(GetTransientPackage(), TEXT("TileCursorDebugDrawComponent"));
	EditorModeSettings = NewObject<UTileSystemEditorModeSettings>(GetTransientPackage(), TEXT("TileSystemEditorModeSettings"));
	TileCursorDrawComponent->SetTileDrawComponent(TileDrawComponent);

	EdWidgetBlueprint = nullptr;
	EditedActor = nullptr;
	EditedTileSystem = nullptr;
}

FTileSystemEdMode::~FTileSystemEdMode()
{
}

void FTileSystemEdMode::AddReferencedObjects(FReferenceCollector& Collector)
{
	// Call parent implementation
	FEdMode::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(TileDrawComponent);
	Collector.AddReferencedObject(TileCursorDrawComponent);
	Collector.AddReferencedObject(EditorModeSettings);
	Collector.AddReferencedObject(EdWidgetBlueprint);
}

bool FTileSystemEdMode::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY)
{
	// Compute a world space ray from the screen space mouse coordinates
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(ViewportClient->IsRealtime()));

	FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
	FViewportCursorLocation MouseViewportRay(View, ViewportClient, MouseX, MouseY);
	FVector TraceDirection = MouseViewportRay.GetDirection();

	FVector TraceStart = MouseViewportRay.GetOrigin();
	if (ViewportClient->IsOrtho())
	{
		TraceStart += -WORLD_MAX * TraceDirection;
	}

	FIntVector TraceLocation;
	if (CursorTrace(ViewportClient, TraceStart, TraceDirection, TraceLocation))
	{
		TPair<FIntVector, FIntVector> Selection = TileCursorDrawComponent->GetTileSelection();
		Selection.Key = TraceLocation;
		Selection.Value = TraceLocation;
		TileCursorDrawComponent->SetTileSelection(Selection);
		TileCursorDrawComponent->MarkRenderStateDirty();
	}

    return FEdMode::MouseMove(ViewportClient, Viewport, MouseX, MouseY);
}

bool FTileSystemEdMode::CapturedMouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 MouseX, int32 MouseY)
{
	// Compute a world space ray from the screen space mouse coordinates
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(ViewportClient->IsRealtime()));

	FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
	FViewportCursorLocation MouseViewportRay(View, ViewportClient, MouseX, MouseY);
	FVector TraceDirection = MouseViewportRay.GetDirection();

	FVector TraceStart = MouseViewportRay.GetOrigin();
	if (ViewportClient->IsOrtho())
	{
		TraceStart += -WORLD_MAX * TraceDirection;
	}

	FIntVector TraceLocation;
	if (CursorTrace(ViewportClient, TraceStart, TraceDirection, TraceLocation))
	{
		TPair<FIntVector, FIntVector> Selection = TileCursorDrawComponent->GetTileSelection();
		if (bIsDragging)
		{
			TRange<FIntVector> ValueRange = TRange<FIntVector>(Selection.Key - EditorModeSettings->DragLimit, Selection.Key + EditorModeSettings->DragLimit);
			Selection.Value = TraceLocation;
			Selection.Value.X = FMath::Clamp(Selection.Value.X, ValueRange.GetLowerBoundValue().X, ValueRange.GetUpperBoundValue().X);
			Selection.Value.Y = FMath::Clamp(Selection.Value.Y, ValueRange.GetLowerBoundValue().Y, ValueRange.GetUpperBoundValue().Y);
			Selection.Value.Z = FMath::Clamp(Selection.Value.Z, ValueRange.GetLowerBoundValue().Z, ValueRange.GetUpperBoundValue().Z);

			TileCursorDrawComponent->SetTileSelection(Selection);
			TileCursorDrawComponent->MarkRenderStateDirty();
		}
	}

	return false;
}

bool FTileSystemEdMode::Select(AActor* InActor, bool bInSelected)
{
	if (bInSelected && InActor)
	{
		SetSelectedTileSystem(InActor);
	}

	return false;
}

void FTileSystemEdMode::Enter()
{
	FEdMode::Enter();

	bIsDragging = false;

	TileDrawComponent->SetVisibility(true);
	TileCursorDrawComponent->SetVisibility(true);

	USelection* ActorSelection = GEditor->GetSelectedActors();
	if (ActorSelection && ActorSelection->Num() == 1)
	{
		if (AActor* SelectedActor = ActorSelection->GetTop<AActor>())
		{
			SetSelectedTileSystem(SelectedActor);
		}
	}

	if (const TSubclassOf<UUserWidget> LoadingWidgetClass = UTileSystemEditorSettings::Get()->TileSystemEditorModeToolkitWidget.LoadSynchronous())
	{
		EdWidgetBlueprint = CreateWidget(GetWorld(), LoadingWidgetClass);
		if (EdWidgetBlueprint->GetClass()->ImplementsInterface(UTileSystemToolkitWidgetInterface::StaticClass()))
		{
			FTileSystemToolkitWidgetInfo WidgetInfo;
			WidgetInfo.TileDrawComponent = TileDrawComponent;
			WidgetInfo.EditorModeSettings = EditorModeSettings;
			ITileSystemToolkitWidgetInterface::Execute_SetToolkitInfo(EdWidgetBlueprint, WidgetInfo);
		}
	}

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FTileSystemEdModeToolkit(this));
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FTileSystemEdMode::Exit()
{
	bIsDragging = false;

	TileDrawComponent->SetVisibility(false);
	TileCursorDrawComponent->SetVisibility(false);

	TileDrawComponent->UnregisterComponent();
	TileCursorDrawComponent->UnregisterComponent();

	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();

	FEdMode::Exit();
}

void FTileSystemEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	// The tile system can get deleted and recreated with undo/redoes
	if (EditedActor)
	{
		if (UTileSystemComponent* TileSystem = Cast<UTileSystemComponent>(EditedActor->GetComponentByClass(UTileSystemComponent::StaticClass())))
		{
			if (EditedTileSystem != TileSystem)
			{
				EditedTileSystem = TileSystem;
				TileDrawComponent->MarkRenderStateDirty();
			}
		}
	}

	if (!TileDrawComponent->IsRegistered())
	{
		TileDrawComponent->RegisterComponentWithWorld(ViewportClient->GetWorld());
	}

	if (!TileCursorDrawComponent->IsRegistered())
	{
		TileCursorDrawComponent->RegisterComponentWithWorld(ViewportClient->GetWorld());
	}
}

bool FTileSystemEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey InKey, EInputEvent InEvent)
{
	if (InKey == EKeys::LeftMouseButton && InEvent == IE_Pressed)
	{
		// Only activate tool if we're not already moving the camera and we're not trying to drag a transform widget
		// Not using "if (!ViewportClient->IsMovingCamera())" because it's wrong in ortho viewports :D
		bool bMovingCamera = Viewport->KeyState(EKeys::MiddleMouseButton) || Viewport->KeyState(EKeys::RightMouseButton) || IsAltDown(Viewport);

		if (!bMovingCamera && TileDrawComponent && TileDrawComponent->GetTileSystem())
		{
			bIsDragging = true;
			return true;
		}
	}

	if (InKey == EKeys::LeftMouseButton && InEvent == IE_Released)
	{
		// Only activate tool if we're not already moving the camera and we're not trying to drag a transform widget
		// Not using "if (!ViewportClient->IsMovingCamera())" because it's wrong in ortho viewports :D
		bool bMovingCamera = Viewport->KeyState(EKeys::MiddleMouseButton) || Viewport->KeyState(EKeys::RightMouseButton) || IsAltDown(Viewport);

		if (bIsDragging && !bMovingCamera && TileDrawComponent && TileDrawComponent->GetTileSystem())
		{
			bIsDragging = false;

			TPair<FIntVector, FIntVector> Selection = TileCursorDrawComponent->GetTileSelection();
			FIntVector MinValue;
			MinValue.X = FMath::Min(Selection.Key.X, Selection.Value.X);
			MinValue.Y = FMath::Min(Selection.Key.Y, Selection.Value.Y);
			MinValue.Z = FMath::Min(Selection.Key.Z, Selection.Value.Z);

			FIntVector MaxValue;
			MaxValue.X = FMath::Max(Selection.Key.X, Selection.Value.X);
			MaxValue.Y = FMath::Max(Selection.Key.Y, Selection.Value.Y);
			MaxValue.Z = FMath::Max(Selection.Key.Z, Selection.Value.Z);

			{
				FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "TileSystemMode_EditTransaction", "Tile System Editing"));
				TileDrawComponent->Modify();
				TileDrawComponent->GetTileSystem()->Modify();
				for (int32 X = MinValue.X; X <= MaxValue.X; ++X)
				{
					for (int32 Y = MinValue.Y; Y <= MaxValue.Y; ++Y)
					{
						for (int32 Z = MinValue.Z; Z <= MaxValue.Z; ++Z)
						{
							if (IsCtrlDown(Viewport))
							{
								TileDrawComponent->GetTileSystem()->SetTile(FIntVector(X, Y, Z), EditorModeSettings->BitMask, EditorModeSettings->CtrlLeftClickOperation);
							}
							else
							{
								TileDrawComponent->GetTileSystem()->SetTile(FIntVector(X, Y, Z), EditorModeSettings->BitMask, EditorModeSettings->LeftClickOperation);
							}
						}
					}
				}
				TileDrawComponent->GetTileSystem()->UpdateDirtyTiles();
				TileDrawComponent->MarkRenderStateDirty();
			}

			return true;
		}
	}

	if ((InKey == EKeys::PageDown || InKey == EKeys::PageUp) && InEvent == IE_Pressed)
	{
		if (InKey == EKeys::PageDown)
		{
			EditorModeSettings->ZLevel--;
		}
		if (InKey == EKeys::PageUp)
		{
			EditorModeSettings->ZLevel++;
		}
		return true;
	}

	return false;
}

void FTileSystemEdMode::SetSelectedTileSystem(AActor* Actor)
{
	if (Actor)
	{
		if (UTileSystemComponent* TileSystem = Cast<UTileSystemComponent>(Actor->GetComponentByClass(UTileSystemComponent::StaticClass())))
		{
			EditedActor = Actor;
			EditedTileSystem = TileSystem;

			TileDrawComponent->SetTileSystem(TileSystem);
			TileCursorDrawComponent->MarkRenderStateDirty();
		}
	}
}

bool FTileSystemEdMode::CursorTrace(FEditorViewportClient* ViewportClient, const FVector& InRayOrigin, const FVector& InRayDirection, FIntVector& TileHit)
{
	if (UTileSystemComponent* TileSystem = TileDrawComponent->GetTileSystem())
	{
		FPlane TileSystemPlane(FVector(0.f, 0.f, EditorModeSettings->ZLevel * TileSystem->TileSize), FVector(0.f, 0.f, 1.f));
		if (!FVector::Parallel(FVector(0.f, 0.f, 1.f), InRayDirection))
		{
			FVector RayHitPoint = FMath::RayPlaneIntersection(InRayOrigin, InRayDirection, TileSystemPlane);

			TileHit = FIntVector(RayHitPoint / TileSystem->TileSize);
			TileHit.Z = EditorModeSettings->ZLevel;
			if (RayHitPoint.X < 0.f)
			{
				TileHit.X -= 1;
			}

			if (RayHitPoint.Y < 0.f)
			{
				TileHit.Y -= 1;
			}

			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////
// FTileSystemEdModeToolkit

void FTileSystemEdModeToolkit::Init(const TSharedPtr<class IToolkitHost>& InitToolkitHost)
{
	if (((FTileSystemEdMode*)GetEditorMode())->EdWidgetBlueprint)
	{
		EdWidget = ((FTileSystemEdMode*)GetEditorMode())->EdWidgetBlueprint->TakeWidget();
	}

	FModeToolkit::Init(InitToolkitHost);
}

FName FTileSystemEdModeToolkit::GetToolkitFName() const
{
	return FName();
}

FText FTileSystemEdModeToolkit::GetBaseToolkitName() const
{
	return FText();
}

TSharedPtr<class SWidget> FTileSystemEdModeToolkit::GetInlineContent() const
{
	return EdWidget;
}

void FTileSystemEdModeToolkit::GetToolPaletteNames(TArray<FName>& InPaletteName) const
{
}

FText FTileSystemEdModeToolkit::GetToolPaletteDisplayName(FName PaletteName) const
{
	return FText();
}

void FTileSystemEdModeToolkit::BuildToolPalette(FName PaletteName, FToolBarBuilder& ToolbarBuilder)
{
}

FText FTileSystemEdModeToolkit::GetActiveToolDisplayName() const
{
	return FText();
}

FText FTileSystemEdModeToolkit::GetActiveToolMessage() const
{
	return FText();
}

void FTileSystemEdModeToolkit::OnToolPaletteChanged(FName PaletteName)
{
}