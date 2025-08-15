#pragma once

#include "CoreMinimal.h"
#include "GMCOrganicMovementComponent.h"
#include "ShibaGMCMovement.generated.h"

// Forward declarations
class AShibaCharacter;
class UDebugConsole;

/**
 * Custom GMC Organic Movement Component for Shiba Character
 * Handles dog-specific movement, physics, and network replication
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NAUGHTYSHIBA_API UShibaGMCMovement : public UGMC_OrganicMovementCmp
{
    GENERATED_BODY()

public:
    UShibaGMCMovement();

    // Input flag accessors for character to use
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    void SetWantsToSprint(bool bWants) { bWantsToSprint = bWants; }
    
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")  
    void SetWantsToCrouch(bool bWants) { bWantsToCrouch = bWants; }
    
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    void SetWantsToJump(bool bWants);
    
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    void SetWantsToBark(bool bWants) { bWantsToBark = bWants; }
    
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    void SetWantsToSniff(bool bWants) { bWantsToSniff = bWants; }

    // Getter functions for debugging
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    bool GetWantsToSprint() const { return bWantsToSprint; }
    
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    bool GetWantsToCrouch() const { return bWantsToCrouch; }

    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    void SetWantsToHowl(bool bWants) { bWantsToHowl = bWants; }

    virtual FVector PreProcessInputVector_Implementation(FVector InRawInputVector) override;
    virtual void SetupPlayerInputComponent_Implementation(UInputComponent* PlayerInputComponent) override;
    
protected:
    // GMC Override Functions - Core movement logic
    virtual void BindReplicationData_Implementation() override;
    virtual void PreMovementUpdate_Implementation(float DeltaTime) override;
    virtual void MovementUpdate_Implementation(float DeltaTime) override;
    virtual void PostMovementUpdate_Implementation(float DeltaTime) override;
    
    // Get owning Shiba character
    UFUNCTION(BlueprintCallable, Category = "Shiba Movement")
    AShibaCharacter* GetShibaCharacter() const;

private:
    // Input flags for GMC replication (PRIVATE - implementation details)
    bool bWantsToJump = false;
    bool bWantsToSprint = false;
    bool bWantsToCrouch = false;
    bool bWantsToBark = false;
    bool bWantsToSniff = false;
    bool bWantsToHowl = false;

    // Cached input manager reference
    UPROPERTY()
    class UInputManagerComponent* CachedInputManager;
    
    // Debug console reference
    UPROPERTY()
    UDebugConsole* DebugConsole;

    // Movement state tracking (PRIVATE - implementation details)
    float LastSpeedChangeTime = 0.0f;
    bool bWasMovingLastFrame = false;

    // Movement helper functions (PRIVATE - implementation details)
    void ProcessDogInput(float DeltaTime);

    // Make these public properties accessible to private methods
    friend class AShibaCharacter;
};