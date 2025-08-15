#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/BaseWidget.h"
#include "Blueprint/UserWidget.h"
#include "UIManager.generated.h"

UENUM(BlueprintType)
enum class EUILayer : uint8
{
    Background      UMETA(DisplayName = "Background"),
    Game            UMETA(DisplayName = "Game"),
    Menu            UMETA(DisplayName = "Menu"),
    Popup           UMETA(DisplayName = "Popup"),
    Modal           UMETA(DisplayName = "Modal"),
    System          UMETA(DisplayName = "System")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWidgetAdded, UBaseWidget*, Widget, EUILayer, Layer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWidgetRemoved, UBaseWidget*, Widget, EUILayer, Layer);

/**
 * UI Manager - Game Instance Subsystem
 * Manages all UI widgets and their layering in Naughty Shiba
 */
UCLASS()
class NAUGHTYSHIBA_API UUIManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Subsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Widget creation and management
    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    UBaseWidget* CreateWidget(TSubclassOf<UBaseWidget> WidgetClass, EUILayer Layer = EUILayer::Game);

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    bool AddWidget(UBaseWidget* Widget, EUILayer Layer = EUILayer::Game);

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    bool RemoveWidget(UBaseWidget* Widget);

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void RemoveAllWidgetsFromLayer(EUILayer Layer);

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void RemoveAllWidgets();

    // Widget queries
    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    UBaseWidget* FindWidget(TSubclassOf<UBaseWidget> WidgetClass) const;

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    TArray<UBaseWidget*> GetWidgetsByLayer(EUILayer Layer) const;

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    TArray<UBaseWidget*> GetAllWidgets() const;

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    int32 GetWidgetCount(EUILayer Layer) const;

    // Layer management
    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void SetLayerVisibility(EUILayer Layer, bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    bool IsLayerVisible(EUILayer Layer) const;

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void SetLayerZOrder(EUILayer Layer, int32 ZOrder);

    // Navigation and focus
    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void SetFocusToWidget(UBaseWidget* Widget);

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    UBaseWidget* GetFocusedWidget() const;

    // Game-specific UI shortcuts
    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void ShowMainMenu();

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void HideMainMenu();

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void ShowGameHUD();

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void HideGameHUD();

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void ShowPauseMenu();

    UFUNCTION(BlueprintCallable, Category = "UI Manager")
    void HidePauseMenu();

    // Widget class references (set in Blueprint or C++)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UBaseWidget> MainMenuWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UBaseWidget> GameHUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UBaseWidget> PauseMenuWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Classes")
    TSubclassOf<UBaseWidget> CourageMeterWidgetClass;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "UI Manager Events")
    FOnWidgetAdded OnWidgetAdded;

    UPROPERTY(BlueprintAssignable, Category = "UI Manager Events")
    FOnWidgetRemoved OnWidgetRemoved;

protected:
    // Layer management
    void InitializeLayers();
    int32 GetZOrderForLayer(EUILayer Layer) const;

private:
    // Widget storage by layer (removed UPROPERTY to fix compilation error)
    TMap<EUILayer, TArray<UBaseWidget*>> WidgetsByLayer;

    // Layer visibility
    TMap<EUILayer, bool> LayerVisibility;

    // Z-order for layers
    TMap<EUILayer, int32> LayerZOrders;

    // Currently focused widget
    UPROPERTY()
    UBaseWidget* FocusedWidget;

    // Cached common widgets
    UPROPERTY()
    UBaseWidget* MainMenuWidget;

    UPROPERTY()
    UBaseWidget* GameHUDWidget;

    UPROPERTY()
    UBaseWidget* PauseMenuWidget;

    // Debug console reference
    UPROPERTY()
    class UDebugConsole* DebugConsole;
};