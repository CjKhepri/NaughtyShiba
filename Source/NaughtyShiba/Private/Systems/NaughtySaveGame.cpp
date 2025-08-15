#include "Systems/NaughtySaveGame.h"
#include "Engine/Engine.h"
#include "Misc/DateTime.h"

const FString UNaughtySaveGame::CURRENT_SAVE_VERSION = TEXT("1.0.0");

UNaughtySaveGame::UNaughtySaveGame()
{
    // Set default values
    SaveSlotName = TEXT("DefaultSave");
    SaveSlotIndex = 0;
    SaveVersion = CURRENT_SAVE_VERSION;
    
    // Initialize default player progress
    PlayerProgress.CourageLevel = 100.0f;
    PlayerProgress.MaxCourage = 100.0f;
    PlayerProgress.TotalBarks = 0;
    PlayerProgress.TerritoriesMarked = 0;
    PlayerProgress.TotalPlayTime = 0.0f;
    PlayerProgress.LastSaveLocation = TEXT("Home");
    
    // Initialize default settings
    GameSettings.MasterVolume = 1.0f;
    GameSettings.SFXVolume = 1.0f;
    GameSettings.MusicVolume = 1.0f;
    GameSettings.MouseSensitivity = 1.0f;
    GameSettings.bInvertYAxis = false;
    GameSettings.GraphicsQuality = 3;
    GameSettings.bFullscreen = true;
    GameSettings.ScreenResolution = FIntPoint(1920, 1080);
    
    // Initialize world state
    WorldState.CurrentLevel = TEXT("Home");
    
    UpdateSaveTime();
}

void UNaughtySaveGame::UpdateSaveTime()
{
    SaveTime = FDateTime::Now();
    WorldState.LastSaveTime = SaveTime;
}

bool UNaughtySaveGame::IsValidSave() const
{
    // Basic validation checks
    if (SaveVersion.IsEmpty())
    {
        return false;
    }
    
    if (PlayerProgress.MaxCourage <= 0.0f)
    {
        return false;
    }
    
    if (PlayerProgress.CourageLevel < 0.0f || PlayerProgress.CourageLevel > PlayerProgress.MaxCourage)
    {
        return false;
    }
    
    return true;
}

FString UNaughtySaveGame::GetFormattedSaveTime() const
{
    return SaveTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
}

float UNaughtySaveGame::GetPlayTimeHours() const
{
    return PlayerProgress.TotalPlayTime / 3600.0f; // Convert seconds to hours
}

void UNaughtySaveGame::ValidateData()
{
    // Clamp courage values
    PlayerProgress.CourageLevel = FMath::Clamp(PlayerProgress.CourageLevel, 0.0f, PlayerProgress.MaxCourage);
    PlayerProgress.MaxCourage = FMath::Max(PlayerProgress.MaxCourage, 1.0f);
    
    // Clamp negative values
    PlayerProgress.TotalBarks = FMath::Max(PlayerProgress.TotalBarks, 0);
    PlayerProgress.TerritoriesMarked = FMath::Max(PlayerProgress.TerritoriesMarked, 0);
    PlayerProgress.TotalPlayTime = FMath::Max(PlayerProgress.TotalPlayTime, 0.0f);
    
    // Validate settings
    GameSettings.MasterVolume = FMath::Clamp(GameSettings.MasterVolume, 0.0f, 1.0f);
    GameSettings.SFXVolume = FMath::Clamp(GameSettings.SFXVolume, 0.0f, 1.0f);
    GameSettings.MusicVolume = FMath::Clamp(GameSettings.MusicVolume, 0.0f, 1.0f);
    GameSettings.MouseSensitivity = FMath::Clamp(GameSettings.MouseSensitivity, 0.1f, 5.0f);
    GameSettings.GraphicsQuality = FMath::Clamp(GameSettings.GraphicsQuality, 0, 4);
    
    // Validate screen resolution
    if (GameSettings.ScreenResolution.X <= 0 || GameSettings.ScreenResolution.Y <= 0)
    {
        GameSettings.ScreenResolution = FIntPoint(1920, 1080);
    }
}

void UNaughtySaveGame::MigrateFromOldVersion(const FString& OldVersion)
{
    // Future implementation for save data migration
    // For now, just update the version
    SaveVersion = CURRENT_SAVE_VERSION;
    
    UE_LOG(LogTemp, Warning, TEXT("Save data migrated from version %s to %s"), *OldVersion, *CURRENT_SAVE_VERSION);
}