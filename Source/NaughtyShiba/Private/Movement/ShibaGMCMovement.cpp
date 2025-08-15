#include "Movement/ShibaGMCMovement.h"
#include "Components/InputManagerComponent.h" 
#include "Characters/ShibaCharacter.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"

UShibaGMCMovement::UShibaGMCMovement()
{
    // Set component defaults for dog movement
    PrimaryComponentTick.bCanEverTick = true;
    
    // Set basic movement speed
    MaxDesiredSpeed = 400.0f;
    
    // Reset input flags
    bWantsToJump = false;
    bWantsToSprint = false;
    bWantsToCrouch = false;
    bWantsToBark = false;
    bWantsToSniff = false;
    bWantsToHowl = false;
    
    // Initialize references
    CachedInputManager = nullptr;
    DebugConsole = nullptr;
    
    // Initialize tracking variables
    LastSpeedChangeTime = 0.0f;
    bWasMovingLastFrame = false;
}

void UShibaGMCMovement::SetupPlayerInputComponent_Implementation(UInputComponent* PlayerInputComponent)
{
    // Call parent implementation first (important for GMC)
    Super::SetupPlayerInputComponent_Implementation(PlayerInputComponent);
    
    // Get the character's InputManager
    if (AShibaCharacter* ShibaChar = GetShibaCharacter())
    {
        CachedInputManager = ShibaChar->FindComponentByClass<UInputManagerComponent>();
        
        if (CachedInputManager)
        {
            if (UEnhancedInputComponent* EnhancedComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
            {
                CachedInputManager->SetupInputBindings(EnhancedComp);
            }
        }
    }
}

void UShibaGMCMovement::BindReplicationData_Implementation()
{
    Super::BindReplicationData_Implementation();

    // Network optimized jump replication
    BindBool(
        bWantsToJump,
        EGMC_PredictionMode::ClientAuth_Input,
        EGMC_CombineMode::AlwaysCombine,        // Immediate sending for responsive jumps
        EGMC_SimulationMode::Periodic_Output,   
        EGMC_InterpolationFunction::NearestNeighbour
    );

    // Other abilities can use periodic updates
    BindBool(
        bWantsToSprint,
        EGMC_PredictionMode::ClientAuth_Input,
        EGMC_CombineMode::CombineIfUnchanged,
        EGMC_SimulationMode::Periodic_Output,
        EGMC_InterpolationFunction::NearestNeighbour
    );
    
    BindBool(
        bWantsToCrouch,
        EGMC_PredictionMode::ClientAuth_Input,
        EGMC_CombineMode::CombineIfUnchanged,
        EGMC_SimulationMode::Periodic_Output,
        EGMC_InterpolationFunction::NearestNeighbour
    );
    
    BindBool(
        bWantsToBark,
        EGMC_PredictionMode::ClientAuth_Input,
        EGMC_CombineMode::CombineIfUnchanged,
        EGMC_SimulationMode::Periodic_Output,
        EGMC_InterpolationFunction::NearestNeighbour
    );
    
    BindBool(
        bWantsToSniff,
        EGMC_PredictionMode::ClientAuth_Input,
        EGMC_CombineMode::CombineIfUnchanged,
        EGMC_SimulationMode::Periodic_Output,
        EGMC_InterpolationFunction::NearestNeighbour
    );

    BindBool(
        bWantsToHowl,
        EGMC_PredictionMode::ClientAuth_Input,
        EGMC_CombineMode::CombineIfUnchanged,
        EGMC_SimulationMode::Periodic_Output,
        EGMC_InterpolationFunction::NearestNeighbour
    );
}

void UShibaGMCMovement::PreMovementUpdate_Implementation(float DeltaTime)
{
    // Call parent FIRST - this processes input into ProcessedInputVector
    Super::PreMovementUpdate_Implementation(DeltaTime);

    // Pre-movement setup for Shiba-specific logic
    AShibaCharacter* ShibaChar = GetShibaCharacter();
    if (!ShibaChar)
    {
        return;
    }

    // Process dog-specific input
    ProcessDogInput(DeltaTime);

    // Set speed based on current character setup
    float CurrentSpeed = ShibaChar->GetCurrentMovementSpeed();
    MaxDesiredSpeed = CurrentSpeed;
}

void UShibaGMCMovement::MovementUpdate_Implementation(float DeltaTime)
{
    Super::MovementUpdate_Implementation(DeltaTime);

    AShibaCharacter* ShibaChar = GetShibaCharacter();
    if (!ShibaChar)
    {
        return;
    }

    // Handle jump input
    if (bWantsToJump)
    {
        ShibaChar->StartJump();
        bWantsToJump = false;
    }

    // Handle bark input
    if (bWantsToBark)
    {
        UE_LOG(LogTemp, Warning, TEXT("🎵 GMC: Processing bWantsToBark flag"));
        ShibaChar->StartBark();
        bWantsToBark = false;  // Reset flag immediately
    }

    // Handle sniff input
    if (bWantsToSniff)
    {
        // DEBUG: Show current state before toggle
        if (GEngine)
        {
            FString PlayerType = ShibaChar->IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
            FString CurrentStateStr = ShibaChar->IsInState(EShibaCharacterState::Sniffing) ? TEXT("SNIFFING") : TEXT("NOT_SNIFFING");
        
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
                FString::Printf(TEXT("🟡 [%s] GMC: Processing Sniff (Current: %s)"), *PlayerType, *CurrentStateStr));
        }

        // Simple toggle - each press switches state
        if (ShibaChar->IsInState(EShibaCharacterState::Sniffing))
        {
            if (GEngine)
            {
                FString PlayerType = ShibaChar->IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, 
                    FString::Printf(TEXT("🟣 [%s] GMC: Calling STOP Sniff"), *PlayerType));
            }
            ShibaChar->StopSniffVision();
        }
        else
        {
            if (GEngine)
            {
                FString PlayerType = ShibaChar->IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
                    FString::Printf(TEXT("🟢 [%s] GMC: Calling START Sniff"), *PlayerType));
            }
            ShibaChar->StartSniffVision();
        }
    
        // CRITICAL: Reset flag immediately after processing
        bWantsToSniff = false;
    
        // DEBUG: Confirm flag reset
        if (GEngine)
        {
            FString PlayerType = ShibaChar->IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, 
                FString::Printf(TEXT("🔴 [%s] GMC: Flag RESET"), *PlayerType));
        }
    }

    if (bWantsToHowl)
    {
        UE_LOG(LogTemp, Warning, TEXT("🎵 GMC: Processing bWantsToHowl flag"));
        ShibaChar->StartHowl();
        bWantsToHowl = false;  // Reset flag immediately
    }
}

void UShibaGMCMovement::PostMovementUpdate_Implementation(float DeltaTime)
{
    Super::PostMovementUpdate_Implementation(DeltaTime);

    // Post-movement cleanup and state management
    AShibaCharacter* ShibaChar = GetShibaCharacter();
    if (!ShibaChar)
    {
        return;
    }

    // Track movement state changes
    bool bIsMovingNow = GetVelocity().Size() > 1.0f;
    if (bIsMovingNow != bWasMovingLastFrame)
    {
        LastSpeedChangeTime = GetWorld()->GetTimeSeconds();
        bWasMovingLastFrame = bIsMovingNow;
    }
}

void UShibaGMCMovement::SetWantsToJump(bool bWants) 
{ 
    bWantsToJump = bWants; 
}

AShibaCharacter* UShibaGMCMovement::GetShibaCharacter() const
{
    return Cast<AShibaCharacter>(GetOwner());
}

void UShibaGMCMovement::ProcessDogInput(float DeltaTime)
{
    AShibaCharacter* ShibaChar = GetShibaCharacter();
    if (!ShibaChar)
    {
        return;
    }

    // Handle sprint input
    if (bWantsToSprint && !ShibaChar->IsInState(EShibaCharacterState::Sprinting))
    {
        if (GetVelocity().Size() > 1.0f)
        {
            ShibaChar->StartSprint();
        }
    }
    else if (!bWantsToSprint && ShibaChar->IsInState(EShibaCharacterState::Sprinting))
    {
        ShibaChar->StopSprint();
    }

    // Handle crouch input
    if (bWantsToCrouch && !ShibaChar->IsInState(EShibaCharacterState::Crouching))
    {
        ShibaChar->StartCrouch();
    }
    else if (!bWantsToCrouch && ShibaChar->IsInState(EShibaCharacterState::Crouching))
    {
        ShibaChar->StopCrouch();
    }
}

FVector UShibaGMCMovement::PreProcessInputVector_Implementation(FVector InRawInputVector)
{
    // Call parent to do normal processing
    FVector ProcessedInput = Super::PreProcessInputVector_Implementation(InRawInputVector);

    return ProcessedInput;
}