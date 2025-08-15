#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveInput, FVector2D, MoveVector);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLookInput, FVector2D, LookVector);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBarkPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBarkReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMarkTerritoryPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickUpPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickUpReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSniffVisionPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSniffVisionReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHowlPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHowlReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDefecatePressed);

/**
 * Enhanced Input Manager Component for Naughty Shiba
 * Centralizes all input handling and provides clean event system
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class NAUGHTYSHIBA_API UInputManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInputManagerComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // Input setup and binding
    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetupInputBindings(UEnhancedInputComponent* EnhancedInputComponent);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void EnableInput(APlayerController* PlayerController);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void DisableInput(APlayerController* PlayerController);

    // Input state queries
    UFUNCTION(BlueprintCallable, Category = "Input")
    bool IsSprintPressed() const { return bSprintPressed; }
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    bool IsCrouchPressed() const { return bCrouchPressed; }
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    bool IsJumpPressed() const { return bJumpPressed; }
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    bool IsPickUpPressed() const { return bPickUpPressed; }
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    bool IsSniffVisionActive() const { return bSniffVisionActive; }
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    bool IsHowlPressed() const { return bHowlPressed; }
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    FVector2D GetMoveInput() const { return CurrentMoveInput; }
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    FVector2D GetLookInput() const { return CurrentLookInput; }

    // Event delegates
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnMoveInput OnMoveInput;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnLookInput OnLookInput;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnJumpPressed OnJumpPressed;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnJumpReleased OnJumpReleased;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnSprintPressed OnSprintPressed;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnSprintReleased OnSprintReleased;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnCrouchPressed OnCrouchPressed;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnCrouchReleased OnCrouchReleased;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnBarkPressed OnBarkPressed;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnBarkReleased OnBarkReleased;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnInteractPressed OnInteractPressed;
    
    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnMarkTerritoryPressed OnMarkTerritoryPressed;

    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnPickUpPressed OnPickUpPressed;

    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnPickUpReleased OnPickUpReleased;

    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnSniffVisionPressed OnSniffVisionPressed;

    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnSniffVisionReleased OnSniffVisionReleased;

    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnHowlPressed OnHowlPressed;

    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnHowlReleased OnHowlReleased;

    UPROPERTY(BlueprintAssignable, Category = "Input Events")
    FOnDefecatePressed OnDefecatePressed;

protected:
    // Input Action references
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* MoveAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* LookAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* JumpAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* SprintAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* CrouchAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* BarkAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* InteractAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* MarkTerritoryAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* PickUpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* SniffVisionAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* HowlAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* DefecateAction;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputMappingContext* DefaultMappingContext;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    int32 MappingPriority = 0;

private:
    // Input callback functions
    void HandleMoveInput(const FInputActionValue& Value);
    void HandleLookInput(const FInputActionValue& Value);
    void HandleJumpPressed(const FInputActionValue& Value);
    void HandleJumpReleased(const FInputActionValue& Value);
    void HandleSprintPressed(const FInputActionValue& Value);
    void HandleSprintReleased(const FInputActionValue& Value);
    void HandleCrouchPressed(const FInputActionValue& Value);
    void HandleCrouchReleased(const FInputActionValue& Value);
    void HandleBarkPressed(const FInputActionValue& Value);
    void HandleBarkReleased(const FInputActionValue& Value);
    void HandleInteractPressed(const FInputActionValue& Value);
    void HandleMarkTerritoryPressed(const FInputActionValue& Value);
    void HandlePickUpPressed(const FInputActionValue& Value);
    void HandlePickUpReleased(const FInputActionValue& Value);
    void HandleSniffVisionPressed(const FInputActionValue& Value);
    void HandleSniffVisionReleased(const FInputActionValue& Value);
    void HandleHowlPressed(const FInputActionValue& Value);
    void HandleHowlReleased(const FInputActionValue& Value);
    void HandleDefecatePressed(const FInputActionValue& Value);

    // Input state tracking
    FVector2D CurrentMoveInput = FVector2D::ZeroVector;
    FVector2D CurrentLookInput = FVector2D::ZeroVector;
    bool bJumpPressed = false;
    bool bSprintPressed = false;
    bool bCrouchPressed = false;
    bool bPickUpPressed = false;
    bool bSniffVisionActive = false;
    bool bHowlPressed = false;

    // Component references
    UPROPERTY()
    class UDebugConsole* DebugConsole;
};