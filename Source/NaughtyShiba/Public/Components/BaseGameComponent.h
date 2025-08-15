#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "BaseGameComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentInitialized, UBaseGameComponent*, Component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentActivated, UBaseGameComponent*, Component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComponentDeactivated, UBaseGameComponent*, Component);

/**
 * Base Game Component for Naughty Shiba
 * Provides common functionality for all game-specific components
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class NAUGHTYSHIBA_API UBaseGameComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBaseGameComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // Component lifecycle
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Base Component")
    void OnComponentInitialized();

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    virtual void InitializeComponent();

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    virtual void ActivateComponent();

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    virtual void DeactivateComponent();

    // State queries
    UFUNCTION(BlueprintCallable, Category = "Base Component")
    bool IsComponentInitialized() const { return bIsInitialized; }

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    bool IsComponentActive() const { return bIsActive && IsActive(); }

    // Network support
    UFUNCTION(BlueprintCallable, Category = "Base Component")
    bool HasAuthority() const;

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    bool IsLocallyControlled() const;

    // Debug functionality
    UFUNCTION(BlueprintCallable, Category = "Base Component")
    virtual void LogComponentInfo();

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    virtual FString GetComponentDebugString() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Base Component Events")
    FOnComponentInitialized OnComponentInitializedEvent;

    UPROPERTY(BlueprintAssignable, Category = "Base Component Events")
    FOnComponentActivated OnComponentActivatedEvent;

    UPROPERTY(BlueprintAssignable, Category = "Base Component Events")
    FOnComponentDeactivated OnComponentDeactivatedEvent;

protected:
    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Component")
    bool bAutoInitialize = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Component")
    bool bAutoActivateComponent = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Component")
    float InitializationDelay = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Component")
    bool bRequiresNetworkAuthority = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Component")
    bool bDebugMode = false;

    // Component dependency system
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Component")
    TArray<TSubclassOf<UActorComponent>> RequiredComponents;

    // Internal state
    bool bIsInitialized = false;
    bool bIsActive = false;

    // Helper functions
    UFUNCTION(BlueprintCallable, Category = "Base Component")
    bool CheckRequiredComponents() const;

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    void LogDebug(const FString& Message) const;

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    void LogWarning(const FString& Message) const;

    UFUNCTION(BlueprintCallable, Category = "Base Component")
    void LogError(const FString& Message) const;

private:
    // Debug console reference
    UPROPERTY()
    class UDebugConsole* DebugConsole;

    // Initialization timer
    FTimerHandle InitializationTimerHandle;

    UFUNCTION()
    void DelayedInitialization();
};