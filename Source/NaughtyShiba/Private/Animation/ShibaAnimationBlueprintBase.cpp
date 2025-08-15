#include "Animation/ShibaAnimationBlueprintBase.h"
#include "Characters/ShibaCharacter.h"

UShibaAnimationBlueprintBase::UShibaAnimationBlueprintBase()
{
    // Default constructor - no smoothing needed
}

void UShibaAnimationBlueprintBase::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // Get the Shiba character reference
    ShibaCharacter = Cast<AShibaCharacter>(TryGetPawnOwner());
    
    if (ShibaCharacter)
    {
        // Initialize with current character state
        Speed = ShibaCharacter->GetVelocity().Size();
        PreviousSpeed = Speed;
    }
}

void UShibaAnimationBlueprintBase::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);

    if (!ShibaCharacter)
    {
        return;
    }

    // Update all animation properties
    UpdateMovementProperties();
    UpdateStateProperties();
    UpdateActionProperties();

    // Store current speed for next frame
    PreviousSpeed = Speed;
}

void UShibaAnimationBlueprintBase::UpdateMovementProperties()
{
    // Get raw movement data directly from character
    Speed = ShibaCharacter->GetVelocity().Size();
    Direction = CalculateDirection();

    // Calculate movement state
    bIsMoving = ShibaCharacter->IsMoving();
    bIsGrounded = ShibaCharacter->IsGrounded();

    // Calculate if accelerating (simple comparison)
    bIsAccelerating = (Speed > PreviousSpeed + 1.0f) && bIsMoving;

    // Simple turn in place check
    bIsTurningInPlace = !bIsMoving && FMath::Abs(Direction) > 0.1f;

    // DEBUGGING: Log values for blend space setup
    if (bIsMoving && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, 
            FString::Printf(TEXT("ANIM: Speed=%.1f, Direction=%.2f"), Speed, Direction));
    }
}

void UShibaAnimationBlueprintBase::UpdateStateProperties()
{
    // Get current character state
    CurrentState = ShibaCharacter->GetCharacterState();

    // Update state booleans
    bIsJumping = ShibaCharacter->IsInState(EShibaCharacterState::Jumping);
    bIsFalling = ShibaCharacter->IsInState(EShibaCharacterState::Falling);
    bIsSprinting = ShibaCharacter->IsInState(EShibaCharacterState::Sprinting);
    bIsCrouching = ShibaCharacter->IsInState(EShibaCharacterState::Crouching);

    // Calculate fall speed for landing animations
    if (bIsFalling)
    {
        FallSpeed = FMath::Abs(ShibaCharacter->GetVelocity().Z);
    }
    else
    {
        FallSpeed = 0.0f;
    }
}

void UShibaAnimationBlueprintBase::UpdateActionProperties()
{
    // Update action states directly from character
    bIsBarking = ShibaCharacter->IsInState(EShibaCharacterState::Barking);
    bIsHowling = ShibaCharacter->IsInState(EShibaCharacterState::Howling);
    bIsSniffing = ShibaCharacter->IsInState(EShibaCharacterState::Sniffing);
    bIsCarrying = ShibaCharacter->IsInState(EShibaCharacterState::Carrying);
    bIsDigging = ShibaCharacter->IsInState(EShibaCharacterState::Digging);
}

float UShibaAnimationBlueprintBase::CalculateDirection() const
{
    if (!bIsMoving || !ShibaCharacter)
    {
        return 0.0f;
    }

    // Get character forward vector and velocity
    FVector ForwardVector = ShibaCharacter->GetActorForwardVector();
    FVector RightVector = ShibaCharacter->GetActorRightVector();
    FVector Velocity = ShibaCharacter->GetVelocity();
    
    // Remove Z component for 2D calculation
    ForwardVector.Z = 0.0f;
    RightVector.Z = 0.0f;
    Velocity.Z = 0.0f;
    
    ForwardVector.Normalize();
    RightVector.Normalize();
    Velocity.Normalize();

    // Return the right component (positive = turning right, negative = turning left)
    return FVector::DotProduct(Velocity, RightVector);
}