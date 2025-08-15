#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "BaseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetShown, UBaseWidget*, Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetHidden, UBaseWidget*, Widget);

/**
 * Base Widget class for all UI elements in Naughty Shiba
 * Provides common functionality and standardized behavior
 */
UCLASS(BlueprintType, Blueprintable)
class NAUGHTYSHIBA_API UBaseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UBaseWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    // Widget lifecycle
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Base Widget")
    void OnWidgetInitialized();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Base Widget")
    void OnWidgetShown();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Base Widget")
    void OnWidgetHidden();

    // Show/Hide functionality
    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    virtual void ShowWidget(bool bAnimated = true);

    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    virtual void HideWidget(bool bAnimated = true);

    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    void ToggleWidget(bool bAnimated = true);

    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    bool IsWidgetVisible() const;

    // Animation support
    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    void PlayShowAnimation();

    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    void PlayHideAnimation();

    // Helper functions
    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    void SetTextBlockText(UTextBlock* TextBlock, const FText& NewText);

    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    void SetProgressBarPercent(UProgressBar* ProgressBar, float Percent);

    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    void SetImageTexture(UImage* Image, UTexture2D* Texture);

    // Button helpers
    UFUNCTION(BlueprintCallable, Category = "Base Widget")
    void BindButtonClick(UButton* Button, const FString& FunctionName);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Base Widget Events")
    FOnWidgetShown OnWidgetShownEvent;

    UPROPERTY(BlueprintAssignable, Category = "Base Widget Events")
    FOnWidgetHidden OnWidgetHiddenEvent;

protected:
    // Widget configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Widget")
    bool bAutoInitialize = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Widget")
    bool bHideOnConstruct = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Widget")
    float ShowAnimationDuration = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Widget")
    float HideAnimationDuration = 0.3f;

    // Animation curves (can be set in Blueprint)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Widget")
    UCurveFloat* ShowAnimationCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Widget")
    UCurveFloat* HideAnimationCurve;

private:
    // Internal state
    bool bIsShowing = false;
    bool bIsAnimating = false;

    // Debug console reference
    UPROPERTY()
    class UDebugConsole* DebugConsole;

    // Animation timeline functions
    UFUNCTION()
    void OnShowAnimationUpdate(float Value);

    UFUNCTION()
    void OnShowAnimationFinished();

    UFUNCTION()
    void OnHideAnimationUpdate(float Value);

    UFUNCTION()
    void OnHideAnimationFinished();
};