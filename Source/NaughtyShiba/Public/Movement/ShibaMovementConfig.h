#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShibaMovementConfig.generated.h"

/**
 * Data asset for configuring Shiba movement parameters
 */
UCLASS(BlueprintType)
class NAUGHTYSHIBA_API UShibaMovementConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UShibaMovementConfig();

    // Basic movement speeds
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Basic")
    float WalkSpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Basic")
    float RunSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Basic")
    float SprintSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Basic")
    float CrouchSpeed = 100.0f;

    // Special movement speeds
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Special")
    float SwimSpeed = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Special")
    float SniffSpeed = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Special")
    float CarrySpeed = 300.0f;

    // Jump parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump")
    float JumpVelocity = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump")
    float AirControl = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump")
    float GravityScale = 1.0f;

    // Physics parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Physics")
    float Acceleration = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Physics")
    float Deceleration = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Physics")
    float TurnRate = 720.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Physics")
    float GroundFriction = 8.0f;

    // Collision parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Collision")
    float StandingHeight = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Collision")
    float CrouchHeight = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Collision")
    float CapsuleRadius = 25.0f;

    // Surface response parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Surface")
    float GrassSpeedMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Surface")
    float MudSpeedMultiplier = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Surface")
    float IceSpeedMultiplier = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Surface")
    float WaterSpeedMultiplier = 0.5f;

    // Stamina parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina")
    float SprintStaminaDrain = 20.0f; // Per second

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina")
    float StaminaRegenRate = 15.0f; // Per second

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Stamina")
    float LowStaminaThreshold = 20.0f;
};