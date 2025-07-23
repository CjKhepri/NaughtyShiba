#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "DebugConsole.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNaughtyDebug, Log, All);

/**
 * Debug console system for development and testing
 * Provides in-game console commands and debugging utilities
 */
UCLASS()
class NAUGHTYSHIBA_API UDebugConsole : public UObject
{
    GENERATED_BODY()

public:
    // Singleton access pattern
    static UDebugConsole* GetInstance(UWorld* World);
    
    // Console command execution
    UFUNCTION(CallInEditor)
    void ExecuteCommand(const FString& Command);
    
    // Debug visualization helpers
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DrawDebugSphere(const FVector& Location, float Radius, const FColor& Color = FColor::Blue);
    
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DrawDebugCircle(const FVector& Location, float Radius, const FColor& Color = FColor::Yellow);
    
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ToggleDebugDisplay();
    
    // Logging convenience functions
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void LogInfo(const FString& Message);
    
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void LogWarning(const FString& Message);
    
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void LogError(const FString& Message);

    // Performance monitoring
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void StartPerformanceTimer(const FString& TimerName);
    
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void EndPerformanceTimer(const FString& TimerName);

private:
    // Command handling functions
    void HandleTeleportCommand(const TArray<FString>& Args);
    void HandleSpawnCommand(const TArray<FString>& Args);
    void HandleCourageCommand(const TArray<FString>& Args);
    void HandleHelpCommand(const TArray<FString>& Args);
    
    // Internal state
    static UDebugConsole* Instance;
    TMap<FString, TFunction<void(const TArray<FString>&)>> Commands;
    TMap<FString, double> PerformanceTimers;
    bool bDebugDisplayEnabled = false;
    
    // Initialization
    void InitializeCommands();
    void RegisterCommand(const FString& CommandName, TFunction<void(const TArray<FString>&)> Handler, const FString& Description = TEXT(""));
    
    // Command registry for help system
    TMap<FString, FString> CommandDescriptions;
};