#include "Components/InputManagerComponent.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

UInputManagerComponent::UInputManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Set default mapping priority
    MappingPriority = 0;
    
    // Initialize debug console reference
    DebugConsole = nullptr;
}

void UInputManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize debug console
    if (GetWorld())
    {
        DebugConsole = UDebugConsole::GetInstance(GetWorld());
    }
}

void UInputManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clean up input context
    if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        DisableInput(PC);
    }
    
    Super::EndPlay(EndPlayReason);
}

void UInputManagerComponent::SetupInputBindings(UEnhancedInputComponent* EnhancedInputComponent)
{
    // Bind movement actions
    if (MoveAction)
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UInputManagerComponent::HandleMoveInput);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandleMoveInput);
    }

    if (LookAction)
    {
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UInputManagerComponent::HandleLookInput);
    }

    // Bind digital actions
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleJumpPressed);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandleJumpReleased);
    }

    if (SprintAction)
    {
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleSprintPressed);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandleSprintReleased);
    }

    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleCrouchPressed);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandleCrouchReleased);
    }

    if (BarkAction)
    {
        EnhancedInputComponent->BindAction(BarkAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleBarkPressed);
        EnhancedInputComponent->BindAction(BarkAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandleBarkReleased);
    }

    if (InteractAction)
    {
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleInteractPressed);
    }

    if (MarkTerritoryAction)
    {
        EnhancedInputComponent->BindAction(MarkTerritoryAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleMarkTerritoryPressed);
    }

    if (PickUpAction)
    {
        EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandlePickUpPressed);
        EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandlePickUpReleased);
    }

    if (SniffVisionAction)
    {
        EnhancedInputComponent->BindAction(SniffVisionAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleSniffVisionPressed);
        /*EnhancedInputComponent->BindAction(SniffVisionAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandleSniffVisionReleased);*/
    }

    if (HowlAction)
    {
        EnhancedInputComponent->BindAction(HowlAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleHowlPressed);
        EnhancedInputComponent->BindAction(HowlAction, ETriggerEvent::Completed, this, &UInputManagerComponent::HandleHowlReleased);
    }

    if (DefecateAction)
    {
        EnhancedInputComponent->BindAction(DefecateAction, ETriggerEvent::Started, this, &UInputManagerComponent::HandleDefecatePressed);
    }

    // Add mapping context to Enhanced Input subsystem
    if (DefaultMappingContext)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(DefaultMappingContext, MappingPriority);
            }
        }
    }
}

void UInputManagerComponent::EnableInput(APlayerController* PlayerController)
{
    if (!PlayerController || !DefaultMappingContext)
    {
        if (DebugConsole)
        {
            DebugConsole->LogError(TEXT("Cannot enable input - Player Controller or Mapping Context is null"));
        }
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, MappingPriority);
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(TEXT("Enhanced Input enabled with default mapping context"));
        }
    }
}

void UInputManagerComponent::DisableInput(APlayerController* PlayerController)
{
    if (!PlayerController || !DefaultMappingContext)
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
        Subsystem->RemoveMappingContext(DefaultMappingContext);
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(TEXT("Enhanced Input disabled"));
        }
    }
}

// Input handler implementations
void UInputManagerComponent::HandleMoveInput(const FInputActionValue& Value)
{
    CurrentMoveInput = Value.Get<FVector2D>();
    OnMoveInput.Broadcast(CurrentMoveInput);
}

void UInputManagerComponent::HandleLookInput(const FInputActionValue& Value)
{
    CurrentLookInput = Value.Get<FVector2D>();
    OnLookInput.Broadcast(CurrentLookInput);
}

void UInputManagerComponent::HandleJumpPressed(const FInputActionValue& Value)
{
    bJumpPressed = true;
    OnJumpPressed.Broadcast();
}

void UInputManagerComponent::HandleJumpReleased(const FInputActionValue& Value)
{
    bJumpPressed = false;
    OnJumpReleased.Broadcast();
}

void UInputManagerComponent::HandleSprintPressed(const FInputActionValue& Value)
{
    bSprintPressed = true;
    OnSprintPressed.Broadcast();
}

void UInputManagerComponent::HandleSprintReleased(const FInputActionValue& Value)
{
    bSprintPressed = false;
    OnSprintReleased.Broadcast();
}

void UInputManagerComponent::HandleCrouchPressed(const FInputActionValue& Value)
{
    bCrouchPressed = true;
    OnCrouchPressed.Broadcast();
}

void UInputManagerComponent::HandleCrouchReleased(const FInputActionValue& Value)
{
    bCrouchPressed = false;
    OnCrouchReleased.Broadcast();
}

void UInputManagerComponent::HandleBarkPressed(const FInputActionValue& Value)
{
    // TESTING LOG - Input detection
    UE_LOG(LogTemp, Warning, TEXT("⌨️ INPUT: B key (Bark) pressed"));
    
    OnBarkPressed.Broadcast();
}



void UInputManagerComponent::HandleBarkReleased(const FInputActionValue& Value)
{
    OnBarkReleased.Broadcast();
}

void UInputManagerComponent::HandleInteractPressed(const FInputActionValue& Value)
{
    OnInteractPressed.Broadcast();
}

void UInputManagerComponent::HandleMarkTerritoryPressed(const FInputActionValue& Value)
{
    // TESTING LOG - Input detection
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, 
            TEXT("⌨️ INPUT: T key (Mark Territory) pressed"));
    }
    
    OnMarkTerritoryPressed.Broadcast();
}

void UInputManagerComponent::HandlePickUpPressed(const FInputActionValue& Value)
{
    bPickUpPressed = true;
    OnPickUpPressed.Broadcast();
}

void UInputManagerComponent::HandlePickUpReleased(const FInputActionValue& Value)
{
    bPickUpPressed = false;
    OnPickUpReleased.Broadcast();
}

void UInputManagerComponent::HandleSniffVisionPressed(const FInputActionValue& Value)
{
    bSniffVisionActive = true;
    OnSniffVisionPressed.Broadcast();
}

void UInputManagerComponent::HandleSniffVisionReleased(const FInputActionValue& Value)
{
    bSniffVisionActive = false;
    OnSniffVisionReleased.Broadcast();
}

void UInputManagerComponent::HandleHowlPressed(const FInputActionValue& Value)
{
    // TESTING LOG - Input detection
    UE_LOG(LogTemp, Warning, TEXT("⌨️ INPUT: H key (Howl) pressed"));
    
    bHowlPressed = true;
    OnHowlPressed.Broadcast();
}

void UInputManagerComponent::HandleHowlReleased(const FInputActionValue& Value)
{
    bHowlPressed = false;
    OnHowlReleased.Broadcast();
}

void UInputManagerComponent::HandleDefecatePressed(const FInputActionValue& Value)
{
    // TESTING LOG - Input detection
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, 
            TEXT("⌨️ INPUT: Y key (Defecate) pressed"));
    }
    
    OnDefecatePressed.Broadcast();
}