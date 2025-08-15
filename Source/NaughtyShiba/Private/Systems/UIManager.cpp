#include "Systems/UIManager.h"
#include "Systems/DebugConsole.h"
#include "UI/BaseWidget.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"  // Add this for UGameplayStatics
#include "Blueprint/WidgetBlueprintLibrary.h"  // Add this for widget creation


void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    InitializeLayers();
    
    // Initialize common widget references
    MainMenuWidget = nullptr;
    GameHUDWidget = nullptr;
    PauseMenuWidget = nullptr;
    FocusedWidget = nullptr;
    
    // Initialize debug console reference
    DebugConsole = nullptr;
    
    UE_LOG(LogTemp, Warning, TEXT("UI Manager initialized"));
}

void UUIManager::Deinitialize()
{
    // Clean up all widgets
    RemoveAllWidgets();
    
    // Clear references
    MainMenuWidget = nullptr;
    GameHUDWidget = nullptr;
    PauseMenuWidget = nullptr;
    FocusedWidget = nullptr;
    
    Super::Deinitialize();
    
    UE_LOG(LogTemp, Warning, TEXT("UI Manager deinitialized"));
}

UBaseWidget* UUIManager::CreateWidget(TSubclassOf<UBaseWidget> WidgetClass, EUILayer Layer)
{
    if (!WidgetClass)
    {
        if (DebugConsole)
        {
            DebugConsole->LogError(TEXT("Cannot create widget - Widget class is null"));
        }
        return nullptr;
    }

    // Use UWidgetBlueprintLibrary for safer widget creation
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        {
            // Use NewObject approach for custom widget classes
            if (UBaseWidget* NewWidget = NewObject<UBaseWidget>(PC, WidgetClass))
            {
                // Initialize the widget
                NewWidget->Initialize();
                
                if (AddWidget(NewWidget, Layer))
                {
                    if (DebugConsole)
                    {
                        DebugConsole->LogInfo(FString::Printf(TEXT("Created widget: %s on layer %d"), 
                                                            *WidgetClass->GetName(), (int32)Layer));
                    }
                    return NewWidget;
                }
            }
        }
        else
        {
            if (DebugConsole)
            {
                DebugConsole->LogWarning(TEXT("No player controller found - creating widget without PC context"));
            }
            
            // Fallback: create without player controller context
            if (UBaseWidget* NewWidget = NewObject<UBaseWidget>(GetTransientPackage(), WidgetClass))
            {
                NewWidget->Initialize();
                
                if (AddWidget(NewWidget, Layer))
                {
                    if (DebugConsole)
                    {
                        DebugConsole->LogInfo(FString::Printf(TEXT("Created widget (fallback): %s on layer %d"), 
                                                            *WidgetClass->GetName(), (int32)Layer));
                    }
                    return NewWidget;
                }
            }
        }
    }

    if (DebugConsole)
    {
        DebugConsole->LogError(FString::Printf(TEXT("Failed to create widget: %s"), *WidgetClass->GetName()));
    }
    return nullptr;
}

bool UUIManager::AddWidget(UBaseWidget* Widget, EUILayer Layer)
{
    if (!Widget)
    {
        return false;
    }

    // Add to viewport with appropriate Z-order
    int32 ZOrder = GetZOrderForLayer(Layer);
    Widget->AddToViewport(ZOrder);

    // Add to our tracking
    if (!WidgetsByLayer.Contains(Layer))
    {
        WidgetsByLayer.Add(Layer, TArray<UBaseWidget*>());
    }
    
    WidgetsByLayer[Layer].AddUnique(Widget);

    // Set layer visibility
    if (LayerVisibility.Contains(Layer))
    {
        Widget->SetVisibility(LayerVisibility[Layer] ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    OnWidgetAdded.Broadcast(Widget, Layer);

    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Added widget to layer %d: %s"), 
                                            (int32)Layer, *Widget->GetClass()->GetName()));
    }

    return true;
}

bool UUIManager::RemoveWidget(UBaseWidget* Widget)
{
    if (!Widget)
    {
        return false;
    }

    // Find and remove from layer tracking
    EUILayer FoundLayer = EUILayer::Game;
    bool bFound = false;

    for (auto& LayerPair : WidgetsByLayer)
    {
        if (LayerPair.Value.Contains(Widget))
        {
            LayerPair.Value.Remove(Widget);
            FoundLayer = LayerPair.Key;
            bFound = true;
            break;
        }
    }

    if (bFound)
    {
        // Remove from viewport
        Widget->RemoveFromParent();

        // Clear focus if this widget was focused
        if (FocusedWidget == Widget)
        {
            FocusedWidget = nullptr;
        }

        OnWidgetRemoved.Broadcast(Widget, FoundLayer);

        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Removed widget from layer %d: %s"), 
                                                (int32)FoundLayer, *Widget->GetClass()->GetName()));
        }

        return true;
    }

    return false;
}

void UUIManager::RemoveAllWidgetsFromLayer(EUILayer Layer)
{
    if (WidgetsByLayer.Contains(Layer))
    {
        TArray<UBaseWidget*>& Widgets = WidgetsByLayer[Layer];
        
        // Create copy to avoid modification during iteration
        TArray<UBaseWidget*> WidgetsToRemove = Widgets;
        
        for (UBaseWidget* Widget : WidgetsToRemove)
        {
            RemoveWidget(Widget);
        }

        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Removed all widgets from layer %d"), (int32)Layer));
        }
    }
}

void UUIManager::RemoveAllWidgets()
{
    for (auto& LayerPair : WidgetsByLayer)
    {
        RemoveAllWidgetsFromLayer(LayerPair.Key);
    }

    if (DebugConsole)
    {
        DebugConsole->LogInfo(TEXT("Removed all widgets from all layers"));
    }
}

UBaseWidget* UUIManager::FindWidget(TSubclassOf<UBaseWidget> WidgetClass) const
{
    if (!WidgetClass)
    {
        return nullptr;
    }

    for (const auto& LayerPair : WidgetsByLayer)
    {
        for (UBaseWidget* Widget : LayerPair.Value)
        {
            if (Widget && Widget->GetClass() == WidgetClass)
            {
                return Widget;
            }
        }
    }

    return nullptr;
}

TArray<UBaseWidget*> UUIManager::GetWidgetsByLayer(EUILayer Layer) const
{
    if (WidgetsByLayer.Contains(Layer))
    {
        return WidgetsByLayer[Layer];
    }
    
    return TArray<UBaseWidget*>();
}

TArray<UBaseWidget*> UUIManager::GetAllWidgets() const
{
    TArray<UBaseWidget*> AllWidgets;
    
    for (const auto& LayerPair : WidgetsByLayer)
    {
        AllWidgets.Append(LayerPair.Value);
    }
    
    return AllWidgets;
}

int32 UUIManager::GetWidgetCount(EUILayer Layer) const
{
    if (WidgetsByLayer.Contains(Layer))
    {
        return WidgetsByLayer[Layer].Num();
    }
    
    return 0;
}

void UUIManager::SetLayerVisibility(EUILayer Layer, bool bVisible)
{
    LayerVisibility.Add(Layer, bVisible);
    
    if (WidgetsByLayer.Contains(Layer))
    {
        for (UBaseWidget* Widget : WidgetsByLayer[Layer])
        {
            if (Widget)
            {
                Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
            }
        }
    }

    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Set layer %d visibility: %s"), 
                                            (int32)Layer, bVisible ? TEXT("Visible") : TEXT("Hidden")));
    }
}

bool UUIManager::IsLayerVisible(EUILayer Layer) const
{
    if (LayerVisibility.Contains(Layer))
    {
        return LayerVisibility[Layer];
    }
    
    return true; // Default to visible
}

void UUIManager::SetLayerZOrder(EUILayer Layer, int32 ZOrder)
{
    LayerZOrders.Add(Layer, ZOrder);
    
    // Update Z-order for all widgets in this layer
    if (WidgetsByLayer.Contains(Layer))
    {
        for (UBaseWidget* Widget : WidgetsByLayer[Layer])
        {
            if (Widget)
            {
                Widget->RemoveFromParent();
                Widget->AddToViewport(ZOrder);
            }
        }
    }
}

void UUIManager::SetFocusToWidget(UBaseWidget* Widget)
{
    FocusedWidget = Widget;
    
    if (Widget)
    {
        Widget->SetFocus();
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Focus set to widget: %s"), *Widget->GetClass()->GetName()));
        }
    }
}

UBaseWidget* UUIManager::GetFocusedWidget() const
{
    return FocusedWidget;
}

void UUIManager::ShowMainMenu()
{
    if (!MainMenuWidget && MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget(MainMenuWidgetClass, EUILayer::Menu);
    }
    
    if (MainMenuWidget)
    {
        MainMenuWidget->ShowWidget();
    }
}

void UUIManager::HideMainMenu()
{
    if (MainMenuWidget)
    {
        MainMenuWidget->HideWidget();
    }
}

void UUIManager::ShowGameHUD()
{
    if (!GameHUDWidget && GameHUDWidgetClass)
    {
        GameHUDWidget = CreateWidget(GameHUDWidgetClass, EUILayer::Game);
    }
    
    if (GameHUDWidget)
    {
        GameHUDWidget->ShowWidget();
    }
}

void UUIManager::HideGameHUD()
{
    if (GameHUDWidget)
    {
        GameHUDWidget->HideWidget();
    }
}

void UUIManager::ShowPauseMenu()
{
    if (!PauseMenuWidget && PauseMenuWidgetClass)
    {
        PauseMenuWidget = CreateWidget(PauseMenuWidgetClass, EUILayer::Modal);
    }
    
    if (PauseMenuWidget)
    {
        PauseMenuWidget->ShowWidget();
    }
}

void UUIManager::HidePauseMenu()
{
    if (PauseMenuWidget)
    {
        PauseMenuWidget->HideWidget();
    }
}

void UUIManager::InitializeLayers()
{
    // Initialize layer visibility (all visible by default)
    LayerVisibility.Add(EUILayer::Background, true);
    LayerVisibility.Add(EUILayer::Game, true);
    LayerVisibility.Add(EUILayer::Menu, true);
    LayerVisibility.Add(EUILayer::Popup, true);
    LayerVisibility.Add(EUILayer::Modal, true);
    LayerVisibility.Add(EUILayer::System, true);
    
    // Initialize layer Z-orders
    LayerZOrders.Add(EUILayer::Background, 0);
    LayerZOrders.Add(EUILayer::Game, 10);
    LayerZOrders.Add(EUILayer::Menu, 20);
    LayerZOrders.Add(EUILayer::Popup, 30);
    LayerZOrders.Add(EUILayer::Modal, 40);
    LayerZOrders.Add(EUILayer::System, 50);
}

int32 UUIManager::GetZOrderForLayer(EUILayer Layer) const
{
    if (LayerZOrders.Contains(Layer))
    {
        return LayerZOrders[Layer];
    }
    
    return 10; // Default Z-order
}