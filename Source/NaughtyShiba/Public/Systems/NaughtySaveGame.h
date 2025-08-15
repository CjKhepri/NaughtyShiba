#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Engine/World.h"
#include "NaughtySaveGame.generated.h"

USTRUCT(BlueprintType)
struct NAUGHTYSHIBA_API FPlayerProgressData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    float CourageLevel = 100.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    float MaxCourage = 100.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    int32 TotalBarks = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    int32 TerritoriesMarked = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    TArray<FString> UnlockedAreas;

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    TArray<FString> CompletedMissions;

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    float TotalPlayTime = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Player Progress")
    FString LastSaveLocation;
};

USTRUCT(BlueprintType)
struct NAUGHTYSHIBA_API FWorldStateData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TArray<FVector> TerritoryMarkers;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TMap<FString, bool> WorldObjects;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    TMap<FString, int32> NPCReputation;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    FDateTime LastSaveTime;

    UPROPERTY(BlueprintReadWrite, Category = "World State")
    FString CurrentLevel;
};

USTRUCT(BlueprintType)
struct NAUGHTYSHIBA_API FGameSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    float MasterVolume = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    float SFXVolume = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    float MusicVolume = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    float MouseSensitivity = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    bool bInvertYAxis = false;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    int32 GraphicsQuality = 3; // 0-4 (Low to Epic)

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    bool bFullscreen = true;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    FIntPoint ScreenResolution = FIntPoint(1920, 1080);
};

/**
 * Save Game class for Naughty Shiba
 * Handles all persistent data including player progress, world state, and settings
 */
UCLASS()
class NAUGHTYSHIBA_API UNaughtySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UNaughtySaveGame();

    // Save slot information
    UPROPERTY(BlueprintReadWrite, Category = "Save Info")
    FString SaveSlotName;

    UPROPERTY(BlueprintReadWrite, Category = "Save Info")
    int32 SaveSlotIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Save Info")
    FDateTime SaveTime;

    UPROPERTY(BlueprintReadWrite, Category = "Save Info")
    FString SaveVersion;

    // Core save data
    UPROPERTY(BlueprintReadWrite, Category = "Save Data")
    FPlayerProgressData PlayerProgress;

    UPROPERTY(BlueprintReadWrite, Category = "Save Data")
    FWorldStateData WorldState;

    UPROPERTY(BlueprintReadWrite, Category = "Save Data")
    FGameSettings GameSettings;

    // Save game functionality
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void UpdateSaveTime();

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool IsValidSave() const;

    UFUNCTION(BlueprintCallable, Category = "Save System")
    FString GetFormattedSaveTime() const;

    UFUNCTION(BlueprintCallable, Category = "Save System")
    float GetPlayTimeHours() const;

    // Data validation and migration
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void ValidateData();

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void MigrateFromOldVersion(const FString& OldVersion);

private:
    // Current save system version for migration support
    static const FString CURRENT_SAVE_VERSION;
};