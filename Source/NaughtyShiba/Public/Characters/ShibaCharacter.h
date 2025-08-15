#pragma once

#include "CoreMinimal.h"
#include "GMCPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GMCFlatCapsuleComponent.h"
#include "Engine/Engine.h"
#include "ShibaCharacter.generated.h"

// Forward declarations
class UInputManagerComponent;
class UShibaGMCMovement;
class USkeletalMeshComponent;


UENUM(BlueprintType)
enum class EShibaCharacterState : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    Walking         UMETA(DisplayName = "Walking"),
    Running         UMETA(DisplayName = "Running"),
    Sprinting      UMETA(DisplayName = "Sprinting"),
    Jumping        UMETA(DisplayName = "Jumping"),
    Falling        UMETA(DisplayName = "Falling"),
    Crouching      UMETA(DisplayName = "Crouching"),
    Barking        UMETA(DisplayName = "Barking"),
    Howling        UMETA(DisplayName = "Howling"),
    Sniffing       UMETA(DisplayName = "Sniffing"),
    MarkingTerritory UMETA(DisplayName = "Marking Territory"),
    Defecating     UMETA(DisplayName = "Defecating"),
    PickingUp      UMETA(DisplayName = "Picking Up"),
    Carrying       UMETA(DisplayName = "Carrying"),
    Swimming       UMETA(DisplayName = "Swimming"),
    Digging        UMETA(DisplayName = "Digging"),
    Dead           UMETA(DisplayName = "Dead")
};



/**
 * Shiba Character class using GMCv2 for movement and replication
 * This is the main playable character in Naughty Shiba
 * Uses HORIZONTAL FLAT CAPSULE collision for proper dog proportions
 */
UCLASS()
class NAUGHTYSHIBA_API AShibaCharacter : public AGMC_Pawn
{
    GENERATED_BODY()

public:
    AShibaCharacter(const FObjectInitializer& ObjectInitializer);

protected:
    // Actor lifecycle
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;

    // Input setup
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

    // State machine
    virtual void OnStateChanged(EShibaCharacterState OldState, EShibaCharacterState NewState);
    virtual bool CanTransitionToState(EShibaCharacterState NewState) const;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|State Thresholds")
    float RunThreshold = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|State Thresholds")
    float SprintThreshold = 600.0f;
    
    // State management
    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void SetCharacterState(EShibaCharacterState NewState);

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    EShibaCharacterState GetCharacterState() const { return CurrentState; }

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    bool IsInState(EShibaCharacterState State) const { return CurrentState == State; }
    
    // Movement queries
    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    bool IsMoving() const;

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    bool IsGrounded() const;
    
    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    bool IsSwimming() const;

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    float GetCurrentSpeed() const;

    // GetVelocity override (no UFUNCTION - inherited from AActor)
    virtual FVector GetVelocity() const override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed Settings")
    float BaseMovementSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed Settings")
    float SprintMultiplier = 1.5f;  

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed Settings")  
    float WalkMultiplier = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed Settings")
    float CrouchMultiplier = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
    float JumpVelocity = 450.0f;

    UFUNCTION(BlueprintCallable, Category = "Movement")  
    float GetCurrentMovementSpeed() const 
    {
        if (bIsCrouching) return BaseMovementSpeed * CrouchMultiplier;
        if (bIsSprinting) return BaseMovementSpeed * SprintMultiplier;  
        if (bIsWalking) return BaseMovementSpeed * WalkMultiplier;
        return BaseMovementSpeed;
    }

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsWalking = false;
    
    // Dog abilities
    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StartJump();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StopJump();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StartSprint();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StopSprint();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StartCrouch();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StopCrouch();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StartBark();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StopBark();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StartHowl();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StopHowl();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StartSniffVision();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StopSniffVision();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void MarkTerritory();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void Defecate();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void StartPickUp();

    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void Interact();
    
    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    UShibaGMCMovement* GetGMCMovementComponent() const;

    // Collision component accessor - FLAT CAPSULE FOR DOG
    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    UGMC_FlatCapsuleCmp* GetFlatCapsule() const { return FlatCapsule; }

    // Input handlers
    UFUNCTION(BlueprintCallable, Category = "Shiba Character")
    void AddLookInput(FVector2D LookAxis);

    // Blueprint events
    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnStateChangedBP(EShibaCharacterState OldState, EShibaCharacterState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnJumped();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnLanded();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnBarked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnHowled();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnTerritoryMarked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnObjectPickedUp(AActor* PickedUpObject);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shiba Character")
    void OnObjectReleased(AActor* ReleasedObject);

protected:

    // ROOT COMPONENT: GMC Flat Capsule (HORIZONTAL for dogs!)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UGMC_FlatCapsuleCmp* FlatCapsule;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* ShibaMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* ThirdPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInputManagerComponent* InputManager = nullptr;

    // GMC Movement Component (NOT the root component)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UShibaGMCMovement* GMCMovementComponent;

    // State management
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "State")
    EShibaCharacterState CurrentState = EShibaCharacterState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    EShibaCharacterState PreviousState = EShibaCharacterState::Idle;

    // Camera settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraDistance = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector CameraOffset = FVector(0.0f, 0.0f, 50.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraLagSpeed = 10.0f;

    // Action states
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsJumping = false;

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsSprinting = false;

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsCrouching = false;

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsBarking = false;

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsHowling = false;

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsSniffing = false;

    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    bool bIsCarryingObject = false;

    // Carried object reference
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Actions")
    AActor* CarriedObject = nullptr;

    // Network replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Helper functions
    virtual void UpdateCharacterState();
    virtual bool IsAnyActionActive() const;
    virtual void CancelAllActions();

private:
    
    bool bInputEventsAlreadyBound = false;
    
    // Debug console reference
    UPROPERTY()
    class UDebugConsole* DebugConsole;

    // Internal state tracking
    float LastMovementTime = 0.0f;
    FVector LastValidLocation = FVector::ZeroVector;
    bool bWasGrounded = true;

    // Input event handlers
    UFUNCTION()
    void HandleMoveInput(FVector2D MoveVector);
    
    UFUNCTION()
    void HandleLookInput(FVector2D LookVector);
    
    UFUNCTION()
    void HandleJumpPressed();
    
    UFUNCTION()
    void HandleSprintPressed();
    
    UFUNCTION()
    void HandleSprintReleased();
    
    UFUNCTION()
    void HandleCrouchPressed();
    
    UFUNCTION()
    void HandleCrouchReleased();
    
    UFUNCTION()
    void HandleBarkPressed();
    
    UFUNCTION()
    void HandleInteractPressed();
    
    UFUNCTION()
    void HandleMarkTerritoryPressed();
    
    UFUNCTION()
    void HandlePickUpPressed();
    
    UFUNCTION()
    void HandleSniffVisionPressed();
    
    UFUNCTION()
    void HandleSniffVisionReleased();
    
    UFUNCTION()
    void HandleHowlPressed();
    
    UFUNCTION()
    void HandleDefecatePressed();
    
};