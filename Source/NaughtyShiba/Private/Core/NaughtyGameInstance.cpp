#include "Core/NaughtyGameInstance.h"
#include "Systems/SaveSystemManager.h"
#include "Systems/UIManager.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"

UNaughtyGameInstance::UNaughtyGameInstance()
{
    bSystemsInitialized = false;
    DebugConsole = nullptr;
}

void UNaughtyGameInstance::Init()
{
    Super::Init();
    
    UE_LOG(LogTemp, Warning, TEXT("=== Naughty Game Instance Init Started ==="));
    
    // Try to get the Save System Manager
    USaveSystemManager* SaveManager = GetSaveSystemManager();
    if (SaveManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("Save System Manager found - enabling auto-save"));
        SaveManager->EnableAutoSave(300.0f);
        
        // Verify it was enabled
        bool bIsEnabled = SaveManager->IsAutoSaveEnabled();
        UE_LOG(LogTemp, Warning, TEXT("Auto-save enabled status: %s"), bIsEnabled ? TEXT("TRUE") : TEXT("FALSE"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR: Save System Manager NOT FOUND in Game Instance Init"));
    }
    
    bSystemsInitialized = true;
    UE_LOG(LogTemp, Warning, TEXT("=== Naughty Game Instance Init Complete ==="));
}

void UNaughtyGameInstance::Shutdown()
{
    UE_LOG(LogTemp, Warning, TEXT("Naughty Game Instance shutting down"));
    
    // Shutdown systems
    ShutdownGameSystems();
    
    Super::Shutdown();
}

void UNaughtyGameInstance::OnStart()
{
    Super::OnStart();
    
    // Additional startup logic can go here
    LogSystemsStatus();
}

USaveSystemManager* UNaughtyGameInstance::GetSaveSystemManager() const
{
    return GetSubsystem<USaveSystemManager>();
}

UUIManager* UNaughtyGameInstance::GetUIManager() const
{
    return GetSubsystem<UUIManager>();
}

void UNaughtyGameInstance::InitializeGameSystems()
{
    if (bSystemsInitialized)
    {
        return;
    }

    // Initialize Save System
    if (USaveSystemManager* SaveManager = GetSaveSystemManager())
    {
        // Enable auto-save with 5 minute intervals
        SaveManager->EnableAutoSave(300.0f);
        UE_LOG(LogTemp, Warning, TEXT("Save System initialized"));
    }

    // Initialize UI System
    if (UUIManager* UIManager = GetUIManager())
    {
        // UI system initialization happens automatically
        UE_LOG(LogTemp, Warning, TEXT("UI System initialized"));
    }

    bSystemsInitialized = true;
}

void UNaughtyGameInstance::ShutdownGameSystems()
{
    if (!bSystemsInitialized)
    {
        return;
    }

    // Shutdown Save System
    if (USaveSystemManager* SaveManager = GetSaveSystemManager())
    {
        SaveManager->DisableAutoSave();
        UE_LOG(LogTemp, Warning, TEXT("Save System shut down"));
    }

    // UI System shuts down automatically

    bSystemsInitialized = false;
}

void UNaughtyGameInstance::LogSystemsStatus()
{
    UE_LOG(LogTemp, Warning, TEXT("=== Game Instance Systems Status ==="));
    UE_LOG(LogTemp, Warning, TEXT("Save System: %s"), GetSaveSystemManager() ? TEXT("Active") : TEXT("Inactive"));
    UE_LOG(LogTemp, Warning, TEXT("UI System: %s"), GetUIManager() ? TEXT("Active") : TEXT("Inactive"));
    UE_LOG(LogTemp, Warning, TEXT("Systems Initialized: %s"), bSystemsInitialized ? TEXT("Yes") : TEXT("No"));
}