#include "UI/BaseWidget.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/CanvasPanelSlot.h"

UBaseWidget::UBaseWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Initialize default values
    bIsShowing = false;
    bIsAnimating = false;
    DebugConsole = nullptr;
}

void UBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize debug console
    if (GetWorld())
    {
        DebugConsole = UDebugConsole::GetInstance(GetWorld());
    }

    // Auto-initialize if enabled
    if (bAutoInitialize)
    {
        OnWidgetInitialized();
    }

    // Hide on construct if specified
    if (bHideOnConstruct)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        bIsShowing = false;
    }
    else
    {
        bIsShowing = true;
    }

    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Widget constructed: %s"), *GetClass()->GetName()));
    }
}

void UBaseWidget::NativeDestruct()
{
    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Widget destructed: %s"), *GetClass()->GetName()));
    }

    Super::NativeDestruct();
}

void UBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // Any per-frame updates can be added here
}

void UBaseWidget::ShowWidget(bool bAnimated)
{
    if (bIsShowing || bIsAnimating)
    {
        return;
    }

    bIsShowing = true;
    SetVisibility(ESlateVisibility::Visible);

    if (bAnimated)
    {
        PlayShowAnimation();
    }

    OnWidgetShown();
    OnWidgetShownEvent.Broadcast(this);

    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Widget shown: %s"), *GetClass()->GetName()));
    }
}

void UBaseWidget::HideWidget(bool bAnimated)
{
    if (!bIsShowing || bIsAnimating)
    {
        return;
    }

    bIsShowing = false;

    if (bAnimated)
    {
        PlayHideAnimation();
    }
    else
    {
        SetVisibility(ESlateVisibility::Collapsed);
    }

    OnWidgetHidden();
    OnWidgetHiddenEvent.Broadcast(this);

    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Widget hidden: %s"), *GetClass()->GetName()));
    }
}

void UBaseWidget::ToggleWidget(bool bAnimated)
{
    if (bIsShowing)
    {
        HideWidget(bAnimated);
    }
    else
    {
        ShowWidget(bAnimated);
    }
}

bool UBaseWidget::IsWidgetVisible() const
{
    return bIsShowing && GetVisibility() == ESlateVisibility::Visible;
}

void UBaseWidget::PlayShowAnimation()
{
    if (bIsAnimating)
    {
        return;
    }

    bIsAnimating = true;

    // Simple fade-in animation
    SetRenderOpacity(0.0f);
    
    // Use a simple interpolation for now
    // This can be enhanced with UMG animations later
    OnShowAnimationUpdate(1.0f);
    OnShowAnimationFinished();
}

void UBaseWidget::PlayHideAnimation()
{
    if (bIsAnimating)
    {
        return;
    }

    bIsAnimating = true;

    // Simple fade-out animation
    OnHideAnimationUpdate(0.0f);
    OnHideAnimationFinished();
}

void UBaseWidget::SetTextBlockText(UTextBlock* TextBlock, const FText& NewText)
{
    if (TextBlock)
    {
        TextBlock->SetText(NewText);
    }
}

void UBaseWidget::SetProgressBarPercent(UProgressBar* ProgressBar, float Percent)
{
    if (ProgressBar)
    {
        ProgressBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
    }
}

void UBaseWidget::SetImageTexture(UImage* Image, UTexture2D* Texture)
{
    if (Image && Texture)
    {
        Image->SetBrushFromTexture(Texture);
    }
}

void UBaseWidget::BindButtonClick(UButton* Button, const FString& FunctionName)
{
    if (Button)
    {
        // This is a simplified implementation
        // In practice, you'd use FOnClicked delegates
        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Button bound to function: %s"), *FunctionName));
        }
    }
}

void UBaseWidget::OnShowAnimationUpdate(float Value)
{
    SetRenderOpacity(Value);
}

void UBaseWidget::OnShowAnimationFinished()
{
    bIsAnimating = false;
    SetRenderOpacity(1.0f);
}

void UBaseWidget::OnHideAnimationUpdate(float Value)
{
    SetRenderOpacity(Value);
}

void UBaseWidget::OnHideAnimationFinished()
{
    bIsAnimating = false;
    SetVisibility(ESlateVisibility::Collapsed);
}