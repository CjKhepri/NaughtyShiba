#include "Characters/ShibaCharacter.h"
#include "Components/InputManagerComponent.h"
#include "Systems/DebugConsole.h"
#include "Movement/ShibaGMCMovement.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "GMCFlatCapsuleComponent.h"
#include "Engine/Engine.h"

AShibaCharacter::AShibaCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    BaseMovementSpeed = 400.0f;
    SprintMultiplier = 1.5f;
    WalkMultiplier = 0.5f;
    CrouchMultiplier = 0.3f;
    JumpVelocity = 450.0f;
    
    // Set default GMCv2 settings
    bReplicates = true;
    bAlwaysRelevant = false;
    NetCullDistanceSquared = 225000000.0f; // 150m squared

    // CRITICAL: Create GMC FLAT CAPSULE as ROOT component (PERFECT FOR DOGS!)
    FlatCapsule = CreateDefaultSubobject<UGMC_FlatCapsuleCmp>(TEXT("FlatCapsule"));
    
    // Configure flat capsule for dog proportions (horizontal capsule)
    FlatCapsule->InitCapsuleSize(30.0f, 20.0f); // Default dog proportions
    FlatCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    FlatCapsule->SetCollisionResponseToAllChannels(ECR_Block);
    FlatCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    FlatCapsule->SetCollisionObjectType(ECC_Pawn);
    
    // SET AS ROOT - This is what GMC requires!
    RootComponent = FlatCapsule;

    // Create custom GMC movement component (NOT the root component)
    GMCMovementComponent = CreateDefaultSubobject<UShibaGMCMovement>(TEXT("GMCMovementComponent"));

    // Create skeletal mesh (attach to flat capsule)
    ShibaMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShibaMesh"));
    ShibaMesh->SetupAttachment(FlatCapsule);
    ShibaMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f)); // Adjust for dog height
    ShibaMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    ShibaMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Create camera boom (attach to flat capsule)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(FlatCapsule);
    CameraBoom->TargetArmLength = CameraDistance;
    CameraBoom->SetRelativeLocation(CameraOffset);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->bInheritPitch = true;
    CameraBoom->bInheritYaw = true;
    CameraBoom->bInheritRoll = false;

    // Create camera
    ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    ThirdPersonCamera->bUsePawnControlRotation = false;
    ThirdPersonCamera->FieldOfView = 90.0f;

    // Initialize state
    CurrentState = EShibaCharacterState::Idle;
    PreviousState = EShibaCharacterState::Idle;

    // Set tick enabled
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AShibaCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Get debug console reference
    if (UWorld* World = GetWorld())
    {
        DebugConsole = UDebugConsole::GetInstance(World);
    }

    // Find the Blueprint InputManager component
    if (!InputManager)
    {
        InputManager = FindComponentByClass<UInputManagerComponent>();
    }

    // Initialize input manager
    if (InputManager && !bInputEventsAlreadyBound)
    {
        // Bind input events to character actions
        InputManager->OnMoveInput.AddDynamic(this, &AShibaCharacter::HandleMoveInput);
        InputManager->OnLookInput.AddDynamic(this, &AShibaCharacter::HandleLookInput);
        InputManager->OnJumpPressed.AddDynamic(this, &AShibaCharacter::HandleJumpPressed);
        InputManager->OnSprintPressed.AddDynamic(this, &AShibaCharacter::HandleSprintPressed);
        InputManager->OnSprintReleased.AddDynamic(this, &AShibaCharacter::HandleSprintReleased);
        InputManager->OnCrouchPressed.AddDynamic(this, &AShibaCharacter::HandleCrouchPressed);
        InputManager->OnCrouchReleased.AddDynamic(this, &AShibaCharacter::HandleCrouchReleased);
        InputManager->OnBarkPressed.AddDynamic(this, &AShibaCharacter::HandleBarkPressed);
        InputManager->OnInteractPressed.AddDynamic(this, &AShibaCharacter::HandleInteractPressed);
        InputManager->OnMarkTerritoryPressed.AddDynamic(this, &AShibaCharacter::HandleMarkTerritoryPressed);
        InputManager->OnPickUpPressed.AddDynamic(this, &AShibaCharacter::HandlePickUpPressed);
        InputManager->OnSniffVisionPressed.AddDynamic(this, &AShibaCharacter::HandleSniffVisionPressed);
        InputManager->OnSniffVisionReleased.AddDynamic(this, &AShibaCharacter::HandleSniffVisionReleased);
        InputManager->OnHowlPressed.AddDynamic(this, &AShibaCharacter::HandleHowlPressed);
        InputManager->OnDefecatePressed.AddDynamic(this, &AShibaCharacter::HandleDefecatePressed);

        bInputEventsAlreadyBound = true;
    }

    // Cache initial location
    LastValidLocation = GetActorLocation();
}

void AShibaCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Cleanup references
    DebugConsole = nullptr;
    CarriedObject = nullptr;

    Super::EndPlay(EndPlayReason);
}

void AShibaCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update character state based on current conditions
    UpdateCharacterState();

    // Track movement time
    if (IsMoving())
    {
        LastMovementTime = GetWorld()->GetTimeSeconds();
    }

    // Cache valid location when grounded
    if (IsGrounded())
    {
        LastValidLocation = GetActorLocation();
        bWasGrounded = true;
    }
    else if (bWasGrounded && !IsGrounded())
    {
        // Just became airborne
        if (CurrentState != EShibaCharacterState::Jumping)
        {
            SetCharacterState(EShibaCharacterState::Falling);
        }
        bWasGrounded = false;
    }
}

void AShibaCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Initialize input context
    if (InputManager && NewController)
    {
        InputManager->EnableInput(Cast<APlayerController>(NewController));
    }
}

void AShibaCharacter::UnPossessed()
{
    if (InputManager && GetController())
    {
        InputManager->DisableInput(Cast<APlayerController>(GetController()));
    }

    Super::UnPossessed();
}

// State Management
void AShibaCharacter::SetCharacterState(EShibaCharacterState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }

    if (!CanTransitionToState(NewState))
    {
        return;
    }

    EShibaCharacterState OldState = CurrentState;
    PreviousState = CurrentState;
    CurrentState = NewState;

    // Call state change handlers
    OnStateChanged(OldState, NewState);
    OnStateChangedBP(OldState, NewState);
}

void AShibaCharacter::OnStateChanged(EShibaCharacterState OldState, EShibaCharacterState NewState)
{
    // Handle state-specific logic for ENTERING states
    switch (NewState)
    {
        case EShibaCharacterState::Jumping:
            bIsJumping = true;
            OnJumped();
            break;
        case EShibaCharacterState::Barking:
            OnBarked();
            break;
        case EShibaCharacterState::Howling:
            OnHowled();
            break;
        case EShibaCharacterState::MarkingTerritory:
            OnTerritoryMarked();
            break;
        default:
            break;
    }

    // Handle state exit logic - Reset bIsJumping when leaving JUMPING state
    switch (OldState)
    {
        case EShibaCharacterState::Jumping:
            bIsJumping = false;  // Reset the flag when leaving JUMPING state
            
            if (IsGrounded())
            {
                OnLanded();
            }
            break;
        case EShibaCharacterState::Falling:
            if (IsGrounded())
            {
                OnLanded();
            }
            break;
        default:
            break;
    }
}

bool AShibaCharacter::CanTransitionToState(EShibaCharacterState NewState) const
{
    // Basic transition rules
    switch (NewState)
    {
    case EShibaCharacterState::Jumping:
        return IsGrounded() && !bIsJumping;
    case EShibaCharacterState::Swimming:
        return IsSwimming();
    case EShibaCharacterState::Dead:
        return true; // Always can die
    default:
        return true; // Most transitions are allowed
    }
}

// Movement Queries
bool AShibaCharacter::IsMoving() const
{
    if (GMCMovementComponent)
    {
        return GMCMovementComponent->GetVelocity().Size() > 10.0f;
    }
    return false;
}

bool AShibaCharacter::IsGrounded() const
{
    if (GMCMovementComponent)
    {
        return GMCMovementComponent->IsMovingOnGround();
    }
    return false;
}

bool AShibaCharacter::IsSwimming() const
{
    if (!GetGMCMovementComponent())
    {
        return false;
    }
    return GetGMCMovementComponent()->IsSwimming();
}

float AShibaCharacter::GetCurrentSpeed() const
{
    if (!GetGMCMovementComponent())
    {
        return 0.0f;
    }
    return GetGMCMovementComponent()->GetVelocity().Size();
}

FVector AShibaCharacter::GetVelocity() const
{
    if (!GetGMCMovementComponent())
    {
        return FVector::ZeroVector;
    }
    return GetGMCMovementComponent()->GetVelocity();
}

UShibaGMCMovement* AShibaCharacter::GetGMCMovementComponent() const
{
    return GMCMovementComponent;
}

// Dog Abilities Implementation
void AShibaCharacter::StartJump()
{
    // Check basic jump conditions
    if (!IsGrounded() || bIsJumping)
    {
        return;
    }

    // Set state FIRST, then set flag
    SetCharacterState(EShibaCharacterState::Jumping);
    bIsJumping = true;
    
    // Apply jump physics
    if (GMCMovementComponent)
    {
        FVector CurrentVelocity = GMCMovementComponent->GetVelocity();
        CurrentVelocity.Z = JumpVelocity;
        GMCMovementComponent->SetVelocity(CurrentVelocity);
    }
}

void AShibaCharacter::StopJump()
{
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToJump(false);
    }
}

void AShibaCharacter::StartSprint()
{
    if (!IsMoving() || bIsSprinting)
    {
        return;
    }

    bIsSprinting = true;
    SetCharacterState(EShibaCharacterState::Sprinting);
    
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToSprint(true);
    }
}

void AShibaCharacter::StopSprint()
{
    bIsSprinting = false;
    
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToSprint(false);
    }
}

void AShibaCharacter::StartCrouch()
{
    if (bIsCrouching)
    {
        return;
    }

    bIsCrouching = true;
    SetCharacterState(EShibaCharacterState::Crouching);

    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToCrouch(true);
    }
}

void AShibaCharacter::StopCrouch()
{
    bIsCrouching = false;

    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToCrouch(false);
    }
}

void AShibaCharacter::StartBark()
{
    // TESTING LOG - Remove after verification
    FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
    UE_LOG(LogTemp, Warning, TEXT("🔵 [%s] BARK: StartBark() called"), *PlayerType);

    if (bIsHowling) return; // Can't bark while howling
    
    bIsBarking = true;
    // UpdateCharacterState() will set the state
    
    // REMOVE THIS LINE - GMC already processed the flag:
    // GMCMovementComponent->SetWantsToBark(true);
    
    // TESTING LOG - Show state change
    UE_LOG(LogTemp, Warning, TEXT("🔷 [%s] BARK: bIsBarking set to true"), *PlayerType);

    // Auto-stop timer
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, this, &AShibaCharacter::StopBark, 1.0f, false);
    }
}

void AShibaCharacter::StopBark()
{
    bIsBarking = false;
}

void AShibaCharacter::StartHowl()
{
    // TESTING LOG - Remove after verification
    FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
    UE_LOG(LogTemp, Warning, TEXT("🟣 [%s] HOWL: StartHowl() called"), *PlayerType);

    if (bIsBarking) return; // Can't howl while barking
    
    bIsHowling = true;
    // UpdateCharacterState() will set the state
    
    // REMOVE THIS LINE - GMC already processed the flag:
    // GMCMovementComponent->SetWantsToHowl(true);
    
    // TESTING LOG - Show state change
    UE_LOG(LogTemp, Warning, TEXT("🟪 [%s] HOWL: bIsHowling set to true"), *PlayerType);

    // Auto-stop timer
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, this, &AShibaCharacter::StopHowl, 3.0f, false);
    }
}

void AShibaCharacter::StopHowl()
{
    bIsHowling = false;
}

void AShibaCharacter::StartSniffVision()
{
    bIsSniffing = true;
    // UpdateCharacterState() will set the state
    
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToSniff(true);
    }

    // TESTING LOG
    if (GEngine)
    {
        FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, 
            FString::Printf(TEXT("🔵 [%s] SNIFF VISION STARTED (Flag Set)"), *PlayerType));
    }
}

void AShibaCharacter::StopSniffVision()
{
    bIsSniffing = false;
    // UpdateCharacterState() will handle state transition
    
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToSniff(false);
    }

    // TESTING LOG
    if (GEngine)
    {
        FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, 
            FString::Printf(TEXT("🟠 [%s] SNIFF VISION STOPPED (Flag Cleared)"), *PlayerType));
    }
}

void AShibaCharacter::MarkTerritory()
{
    // TESTING LOG - Remove after verification
    if (GEngine)
    {
        FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, 
            FString::Printf(TEXT("🟠 [%s] MARK: MarkTerritory() called"), *PlayerType));
    }

    if (!IsGrounded())
    {
        if (GEngine)
        {
            FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
                FString::Printf(TEXT("🔴 [%s] MARK: BLOCKED - Not grounded"), *PlayerType));
        }
        return;
    }

    SetCharacterState(EShibaCharacterState::MarkingTerritory);

    // TESTING LOG - Show state change
    if (GEngine)
    {
        FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
            FString::Printf(TEXT("🟡 [%s] MARK: State set to MarkingTerritory"), *PlayerType));
    }

    // Auto-return to previous state after marking
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            SetCharacterState(EShibaCharacterState::Idle);
            
            // TESTING LOG - Show state return
            if (GEngine)
            {
                FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
                    FString::Printf(TEXT("🟢 [%s] MARK: Returned to Idle"), *PlayerType));
            }
        }, 2.0f, false);
    }
}

void AShibaCharacter::Defecate()
{
    // TESTING LOG - Remove after verification
    if (GEngine)
    {
        FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, 
            FString::Printf(TEXT("🟤 [%s] POOP: Defecate() called"), *PlayerType));
    }

    if (!IsGrounded())
    {
        if (GEngine)
        {
            FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
                FString::Printf(TEXT("🔴 [%s] POOP: BLOCKED - Not grounded"), *PlayerType));
        }
        return;
    }

    SetCharacterState(EShibaCharacterState::Defecating);

    // TESTING LOG - Show state change
    if (GEngine)
    {
        FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, 
            FString::Printf(TEXT("🟠 [%s] POOP: State set to Defecating"), *PlayerType));
    }

    // Auto-return to previous state after action
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            SetCharacterState(EShibaCharacterState::Idle);
            
            // TESTING LOG - Show state return
            if (GEngine)
            {
                FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
                    FString::Printf(TEXT("🟢 [%s] POOP: Returned to Idle"), *PlayerType));
            }
        }, 3.0f, false);
    }
}

void AShibaCharacter::StartPickUp()
{
    if (bIsCarryingObject)
    {
        // Drop current object
        if (CarriedObject)
        {
            OnObjectReleased(CarriedObject);
            CarriedObject = nullptr;
            bIsCarryingObject = false;
        }
        return;
    }

    SetCharacterState(EShibaCharacterState::PickingUp);

    // Auto-return to idle after pickup attempt
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            SetCharacterState(EShibaCharacterState::Idle);
        }, 1.5f, false);
    }
}

void AShibaCharacter::Interact()
{
    // Placeholder for interaction logic
    // Will be expanded in future sections
}

// Input Handlers
void AShibaCharacter::AddLookInput(FVector2D LookAxis)
{
    if (AController* PC = GetController())
    {
        // Apply look input
        if (APlayerController* PlayerController = Cast<APlayerController>(PC))
        {
            PlayerController->AddYawInput(LookAxis.X);
            PlayerController->AddPitchInput(LookAxis.Y);
        }
    }
}

// Network Replication
void AShibaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Replicate character state
    DOREPLIFETIME(AShibaCharacter, CurrentState);
    DOREPLIFETIME(AShibaCharacter, bIsJumping);
    DOREPLIFETIME(AShibaCharacter, bIsSprinting);
    DOREPLIFETIME(AShibaCharacter, bIsWalking);
    DOREPLIFETIME(AShibaCharacter, bIsCrouching);
    DOREPLIFETIME(AShibaCharacter, bIsBarking);
    DOREPLIFETIME(AShibaCharacter, bIsHowling);
    DOREPLIFETIME(AShibaCharacter, bIsSniffing);
    DOREPLIFETIME(AShibaCharacter, bIsCarryingObject);
    DOREPLIFETIME(AShibaCharacter, CarriedObject);
}

// Helper Functions
void AShibaCharacter::UpdateCharacterState()
{
    // PRIORITY 1: Death (always wins)
    /*if (/* death condition #1#)
    {
        SetCharacterState(EShibaCharacterState::Dead);
        return;
    }*/

    // PRIORITY 2: Airborne States (physics-driven, can't be overridden)
    if (CurrentState == EShibaCharacterState::Jumping)
    {
        // Only allow JUMPING → FALLING based on velocity
        if (GetVelocity().Z <= 0.0f)
        {
            SetCharacterState(EShibaCharacterState::Falling);
        }
        return; // Don't check other states while jumping
    }
    
    if (CurrentState == EShibaCharacterState::Falling)
    {
        if (IsGrounded())
        {
            // Fall back to grounded state logic below
        }
        else
        {
            return; // Still falling, don't change state
        }
    }

    // PRIORITY 3: Environmental States (external conditions)
    if (IsSwimming())
    {
        SetCharacterState(EShibaCharacterState::Swimming);
        return;
    }

    // PRIORITY 4: Action States (mutually exclusive abilities)
    // These are one-time actions that interrupt movement
    if (bIsBarking)
    {
        SetCharacterState(EShibaCharacterState::Barking);
        return;
    }
    
    if (bIsHowling)
    {
        SetCharacterState(EShibaCharacterState::Howling);
        return;
    }
    
    if (CurrentState == EShibaCharacterState::MarkingTerritory ||
        CurrentState == EShibaCharacterState::Defecating ||
        CurrentState == EShibaCharacterState::PickingUp ||
        CurrentState == EShibaCharacterState::Digging)
    {
        // These are timer-based states, don't override them
        return;
    }

    // PRIORITY 5: Persistent Modifier States (can combine with movement)
    bool bHasModifier = false;
    
    if (bIsSniffing)
    {
        // DEBUG: Log when UpdateCharacterState sets Sniffing state
        if (CurrentState != EShibaCharacterState::Sniffing && GEngine)
        {
            FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
                FString::Printf(TEXT("🟢 [%s] UpdateCharacterState: Setting SNIFFING State"), *PlayerType));
        }
        
        SetCharacterState(EShibaCharacterState::Sniffing);
        bHasModifier = true;
    }
    else if (CurrentState == EShibaCharacterState::Sniffing)
    {
        // DEBUG: Log when leaving Sniffing state
        if (GEngine)
        {
            FString PlayerType = IsLocallyControlled() ? TEXT("LOCAL") : TEXT("REMOTE");
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
                FString::Printf(TEXT("🔴 [%s] UpdateCharacterState: Leaving SNIFFING State"), *PlayerType));
        }
    }
    
    if (bIsCrouching)
    {
        SetCharacterState(EShibaCharacterState::Crouching);
        bHasModifier = true;
    }
    
    if (bHasModifier)
    {
        return; // Don't override modifier states with movement states
    }

    // PRIORITY 6: Movement States (lowest priority, based on speed)
    if (IsGrounded() && IsMoving())
    {
        float CurrentSpeed = GetCurrentSpeed();
        
        if (bIsSprinting && CurrentSpeed >= SprintThreshold)
        {
            SetCharacterState(EShibaCharacterState::Sprinting);
        }
        else if (CurrentSpeed >= RunThreshold)
        {
            SetCharacterState(EShibaCharacterState::Running);
        }
        else
        {
            SetCharacterState(EShibaCharacterState::Walking);
        }
    }
    else if (IsGrounded())
    {
        SetCharacterState(EShibaCharacterState::Idle);
    }
}

bool AShibaCharacter::IsAnyActionActive() const
{
    return bIsBarking || 
           bIsHowling || 
           CurrentState == EShibaCharacterState::MarkingTerritory ||
           CurrentState == EShibaCharacterState::Defecating ||
           CurrentState == EShibaCharacterState::PickingUp ||
           CurrentState == EShibaCharacterState::Digging ||
           CurrentState == EShibaCharacterState::Sniffing;
}

void AShibaCharacter::CancelAllActions()
{
    if (bIsBarking) StopBark();
    if (bIsHowling) StopHowl();
    if (bIsSniffing) StopSniffVision();
    if (bIsSprinting) StopSprint();
    if (bIsCrouching) StopCrouch();
}

// Input Event Handlers
void AShibaCharacter::HandleMoveInput(FVector2D MoveVector)
{
    if (Controller && GMCMovementComponent)
    {
        // Pass raw input directly - let GMC handle everything
        FVector InputVector3D = FVector(MoveVector.Y, MoveVector.X, 0.0f);
        GMCMovementComponent->AddInputVector(InputVector3D);
    }
}

void AShibaCharacter::HandleLookInput(FVector2D LookVector)
{
    if (Controller)
    {
        AddControllerYawInput(LookVector.X);
        AddControllerPitchInput(-LookVector.Y);
    }
}

void AShibaCharacter::HandleJumpPressed()
{
    // Set GMC flag directly
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToJump(true);
    }
}

void AShibaCharacter::HandleSprintPressed()
{
    StartSprint();
}

void AShibaCharacter::HandleSprintReleased()
{
    StopSprint();
}

void AShibaCharacter::HandleCrouchPressed()
{
    StartCrouch();
}

void AShibaCharacter::HandleCrouchReleased()
{
    StopCrouch();
}

void AShibaCharacter::HandleBarkPressed()
{
    // TESTING LOG - Input detection  
    UE_LOG(LogTemp, Warning, TEXT("⌨️ INPUT: B key (Bark) pressed - Setting GMC flag only"));
    
    // ONLY set GMC flag - let GMC handle calling StartBark()
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToBark(true);
    }
}

void AShibaCharacter::HandleInteractPressed()
{
    Interact();
}

void AShibaCharacter::HandleMarkTerritoryPressed()
{
    MarkTerritory();
}

void AShibaCharacter::HandlePickUpPressed()
{
    StartPickUp();
}

void AShibaCharacter::HandleSniffVisionPressed()
{
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToSniff(true);
    }
}

void AShibaCharacter::HandleSniffVisionReleased()
{
    StopSniffVision();
}

void AShibaCharacter::HandleHowlPressed()
{
    // TESTING LOG - Input detection
    UE_LOG(LogTemp, Warning, TEXT("⌨️ INPUT: H key (Howl) pressed - Setting GMC flag only"));
    
    // ONLY set GMC flag - let GMC handle calling StartHowl()
    if (GMCMovementComponent)
    {
        GMCMovementComponent->SetWantsToHowl(true);
    }
}

void AShibaCharacter::HandleDefecatePressed()
{
    Defecate();
}