#include "Systems/SaveSystemManager.h"
#include "Systems/DebugConsole.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void USaveSystemManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Create default save data
    CurrentSaveData = NewObject<UNaughtySaveGame>(this);
    
    // Initialize debug console reference
    DebugConsole = nullptr;
    
    UE_LOG(LogTemp, Warning, TEXT("=== Save System Manager Initialize Called ==="));
    UE_LOG(LogTemp, Warning, TEXT("Auto-save initially disabled (will be enabled by Game Instance)"));
}

void USaveSystemManager::Deinitialize()
{
    // Disable auto-save
    DisableAutoSave();
    
    // Clear current save data
    CurrentSaveData = nullptr;
    
    Super::Deinitialize();
    
    UE_LOG(LogTemp, Warning, TEXT("Save System Manager deinitialized"));
}

void USaveSystemManager::SaveGame(const FString& SlotName, int32 SlotIndex)
{
    PerformSave(SlotName, SlotIndex);
}

void USaveSystemManager::SaveGameAsync(const FString& SlotName, int32 SlotIndex)
{
    // For now, perform synchronous save
    // Async implementation can be added later for larger save files
    PerformSave(SlotName, SlotIndex);
}

void USaveSystemManager::QuickSave()
{
    SaveGame(DefaultSaveSlot, DefaultSaveIndex);
}

bool USaveSystemManager::LoadGame(const FString& SlotName, int32 SlotIndex)
{
    PerformLoad(SlotName, SlotIndex);
    return CurrentSaveData != nullptr;
}

void USaveSystemManager::LoadGameAsync(const FString& SlotName, int32 SlotIndex)
{
    // For now, perform synchronous load
    PerformLoad(SlotName, SlotIndex);
}

void USaveSystemManager::QuickLoad()
{
    LoadGame(DefaultSaveSlot, DefaultSaveIndex);
}

bool USaveSystemManager::DoesSaveExist(const FString& SlotName, int32 SlotIndex) const
{
    return UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex);
}

bool USaveSystemManager::DeleteSave(const FString& SlotName, int32 SlotIndex)
{
    bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
    
    if (bSuccess)
    {
        OnSaveSlotDeleted.Broadcast(SlotName);
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Deleted save slot: %s [%d]"), *SlotName, SlotIndex));
        }
    }
    
    return bSuccess;
}

TArray<FString> USaveSystemManager::GetAllSaveSlots() const
{
    TArray<FString> SaveSlots;
    
    // Check for common save slot names
    TArray<FString> CommonSlots = {
        TEXT("DefaultSave"),
        TEXT("QuickSave"),
        TEXT("AutoSave"),
        TEXT("Slot1"),
        TEXT("Slot2"),
        TEXT("Slot3")
    };
    
    for (const FString& Slot : CommonSlots)
    {
        if (DoesSaveExist(Slot, 0))
        {
            SaveSlots.Add(Slot);
        }
    }
    
    return SaveSlots;
}

UNaughtySaveGame* USaveSystemManager::GetSaveGameInfo(const FString& SlotName, int32 SlotIndex) const
{
    if (UNaughtySaveGame* LoadedSave = Cast<UNaughtySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex)))
    {
        return LoadedSave;
    }
    
    return nullptr;
}

void USaveSystemManager::SetCurrentSaveData(UNaughtySaveGame* NewSaveData)
{
    if (NewSaveData)
    {
        CurrentSaveData = NewSaveData;
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(TEXT("Current save data updated"));
        }
    }
}

void USaveSystemManager::UpdatePlayerProgress(float NewCourage, int32 NewBarks, int32 NewTerritories)
{
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        NaughtySave->PlayerProgress.CourageLevel = NewCourage;
        NaughtySave->PlayerProgress.TotalBarks = NewBarks;
        NaughtySave->PlayerProgress.TerritoriesMarked = NewTerritories;
    }
}

void USaveSystemManager::AddPlayTime(float AdditionalTime)
{
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        NaughtySave->PlayerProgress.TotalPlayTime += AdditionalTime;
    }
}

void USaveSystemManager::UnlockArea(const FString& AreaName)
{
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        NaughtySave->PlayerProgress.UnlockedAreas.AddUnique(AreaName);
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Area unlocked: %s"), *AreaName));
        }
    }
}

void USaveSystemManager::CompleteMission(const FString& MissionName)
{
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        NaughtySave->PlayerProgress.CompletedMissions.AddUnique(MissionName);
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Mission completed: %s"), *MissionName));
        }
    }
}

void USaveSystemManager::AddTerritoryMarker(const FVector& Location)
{
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        NaughtySave->WorldState.TerritoryMarkers.Add(Location);
    }
}

void USaveSystemManager::UpdateNPCReputation(const FString& NPCName, int32 ReputationChange)
{
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        int32* CurrentRep = NaughtySave->WorldState.NPCReputation.Find(NPCName);
        if (CurrentRep)
        {
            *CurrentRep += ReputationChange;
        }
        else
        {
            NaughtySave->WorldState.NPCReputation.Add(NPCName, ReputationChange);
        }
    }
}

void USaveSystemManager::SetWorldObjectState(const FString& ObjectName, bool bState)
{
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        NaughtySave->WorldState.WorldObjects.Add(ObjectName, bState);
    }
}

void USaveSystemManager::EnableAutoSave(float IntervalSeconds)
{
    AutoSaveInterval = FMath::Max(IntervalSeconds, 60.0f); // Minimum 1 minute
    bAutoSaveEnabled = true;
    
    // Clear existing timer and start new one
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoSaveTimerHandle);
        World->GetTimerManager().SetTimer(AutoSaveTimerHandle, this, &USaveSystemManager::AutoSaveTimer, AutoSaveInterval, true);
    }
    
    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Auto-save enabled: every %.1f seconds"), AutoSaveInterval));
    }
}

void USaveSystemManager::DisableAutoSave()
{
    bAutoSaveEnabled = false;
    
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoSaveTimerHandle);
    }
    
    if (DebugConsole)
    {
        DebugConsole->LogInfo(TEXT("Auto-save disabled"));
    }
}

void USaveSystemManager::PerformSave(const FString& SlotName, int32 SlotIndex)
{
    if (!CurrentSaveData)
    {
        CurrentSaveData = NewObject<UNaughtySaveGame>(this);
    }
    
    // Cast to our save game type
    if (UNaughtySaveGame* NaughtySave = Cast<UNaughtySaveGame>(CurrentSaveData))
    {
        // Update save information
        NaughtySave->SaveSlotName = SlotName;
        NaughtySave->SaveSlotIndex = SlotIndex;
        NaughtySave->UpdateSaveTime();
        NaughtySave->ValidateData();
        
        // Perform the save
        bool bSuccess = UGameplayStatics::SaveGameToSlot(NaughtySave, SlotName, SlotIndex);
        
        if (bSuccess)
        {
            if (DebugConsole)
            {
                DebugConsole->LogInfo(FString::Printf(TEXT("Game saved successfully: %s [%d]"), *SlotName, SlotIndex));
            }
        }
        else
        {
            if (DebugConsole)
            {
                DebugConsole->LogError(FString::Printf(TEXT("Failed to save game: %s [%d]"), *SlotName, SlotIndex));
            }
        }
        
        OnSaveComplete.Broadcast(bSuccess);
    }
}

void USaveSystemManager::PerformLoad(const FString& SlotName, int32 SlotIndex)
{
    if (UNaughtySaveGame* LoadedSave = Cast<UNaughtySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex)))
    {
        LoadedSave->ValidateData();
        CurrentSaveData = LoadedSave;
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Game loaded successfully: %s [%d]"), *SlotName, SlotIndex));
        }
        
        OnLoadComplete.Broadcast(true);
    }
    else
    {
        if (DebugConsole)
        {
            DebugConsole->LogError(FString::Printf(TEXT("Failed to load game: %s [%d]"), *SlotName, SlotIndex));
        }
        
        OnLoadComplete.Broadcast(false);
    }
}

void USaveSystemManager::AutoSaveTimer()
{
    if (bAutoSaveEnabled)
    {
        SaveGame(TEXT("AutoSave"), 0);
    }
}