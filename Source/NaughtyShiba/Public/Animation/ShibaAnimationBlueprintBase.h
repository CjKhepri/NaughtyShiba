#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/ShibaCharacter.h"
#include "ShibaAnimationBlueprintBase.generated.h"

/**
 * Bare bones Animation Blueprint base class for Shiba character
 * Provides raw character data to Blueprint Animation system
 */
UCLASS(BlueprintType, Blueprintable)
class NAUGHTYSHIBA_API UShibaAnimationBlueprintBase : public UAnimInstance
{
    GENERATED_BODY()

public:
    UShibaAnimationBlueprintBase();

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTimeX) override;

public:
    // Character reference
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    AShibaCharacter* ShibaCharacter = nullptr;

    // Movement properties - raw values
    UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
    float Speed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
    float Direction = 0.0f;  // -1.0 = Left, 0.0 = Forward, +1.0 = Right

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
    bool bIsMoving = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
    bool bIsAccelerating = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
    bool bIsTurningInPlace = false;

    // State properties
    UPROPERTY(BlueprintReadOnly, Category = "Animation|State")
    EShibaCharacterState CurrentState = EShibaCharacterState::Idle;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|State")
    bool bIsGrounded = true;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|State")
    bool bIsJumping = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|State")
    bool bIsFalling = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|State")
    float FallSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|State")
    bool bIsSprinting = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|State")
    bool bIsCrouching = false;

    // Action properties
    UPROPERTY(BlueprintReadOnly, Category = "Animation|Actions")
    bool bIsBarking = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Actions")
    bool bIsHowling = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Actions")
    bool bIsSniffing = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Actions")
    bool bIsCarrying = false;

    UPROPERTY(BlueprintReadOnly, Category = "Animation|Actions")
    bool bIsDigging = false;

protected:
    // Update functions called each frame
    void UpdateMovementProperties();
    void UpdateStateProperties();
    void UpdateActionProperties();

    // Helper functions
    float CalculateDirection() const;

private:
    // Previous frame values for acceleration calculation
    float PreviousSpeed = 0.0f;
};