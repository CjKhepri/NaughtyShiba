#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Systems/NaughtySaveGame.h"
#include "Engine/World.h"
#include "SaveSystemManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveSlotDeleted, const FString&, SlotName);

/**
 * Save System Manager - Game Instance Subsystem
 * Handles all save/load operations for Naughty Shiba
 */
UCLASS()
class NAUGHTYSHIBA_API USaveSystemManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Subsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Save operations
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void SaveGame(const FString& SlotName = TEXT("DefaultSave"), int32 SlotIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void SaveGameAsync(const FString& SlotName = TEXT("DefaultSave"), int32 SlotIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void QuickSave();

    // Load operations
    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool LoadGame(const FString& SlotName = TEXT("DefaultSave"), int32 SlotIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void LoadGameAsync(const FString& SlotName = TEXT("DefaultSave"), int32 SlotIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void QuickLoad();

    // Save slot management
    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool DoesSaveExist(const FString& SlotName, int32 SlotIndex = 0) const;

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool DeleteSave(const FString& SlotName, int32 SlotIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    TArray<FString> GetAllSaveSlots() const;

    UFUNCTION(BlueprintCallable, Category = "Save System")
    UNaughtySaveGame* GetSaveGameInfo(const FString& SlotName, int32 SlotIndex = 0) const;

    // Current save data access
    UFUNCTION(BlueprintCallable, Category = "Save System")
    UNaughtySaveGame* GetCurrentSaveData() const { return CurrentSaveData; }

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void SetCurrentSaveData(UNaughtySaveGame* NewSaveData);

    // Player progress shortcuts
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void UpdatePlayerProgress(float NewCourage, int32 NewBarks, int32 NewTerritories);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void AddPlayTime(float AdditionalTime);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void UnlockArea(const FString& AreaName);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void CompleteMission(const FString& MissionName);

    // World state shortcuts
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void AddTerritoryMarker(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void UpdateNPCReputation(const FString& NPCName, int32 ReputationChange);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void SetWorldObjectState(const FString& ObjectName, bool bState);

    // Auto-save functionality
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void EnableAutoSave(float IntervalSeconds = 300.0f);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void DisableAutoSave();

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool IsAutoSaveEnabled() const { return bAutoSaveEnabled; }

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Save System Events")
    FOnSaveComplete OnSaveComplete;

    UPROPERTY(BlueprintAssignable, Category = "Save System Events")
    FOnLoadComplete OnLoadComplete;

    UPROPERTY(BlueprintAssignable, Category = "Save System Events")
    FOnSaveSlotDeleted OnSaveSlotDeleted;

protected:
    // Internal save operations
    void PerformSave(const FString& SlotName, int32 SlotIndex);
    void PerformLoad(const FString& SlotName, int32 SlotIndex);

    // Auto-save timer
    UFUNCTION()
    void AutoSaveTimer();

private:
    // Current save data
    UPROPERTY()
    UNaughtySaveGame* CurrentSaveData;

    // Auto-save settings
    bool bAutoSaveEnabled = false;
    float AutoSaveInterval = 300.0f; // 5 minutes default
    FTimerHandle AutoSaveTimerHandle;

    // Default save slot info
    FString DefaultSaveSlot = TEXT("DefaultSave");
    int32 DefaultSaveIndex = 0;

    // Debug console reference
    UPROPERTY()
    class UDebugConsole* DebugConsole;
};