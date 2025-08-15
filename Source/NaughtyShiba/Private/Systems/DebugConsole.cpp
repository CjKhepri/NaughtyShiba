#include "Systems/DebugConsole.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NaughtyGameState.h"
#include "WorldTime.h"  // GMCv2 WorldTimeReplicator
#include "EngineUtils.h"  // For TActorIterator
#include "Systems/SaveSystemManager.h"
#include "Systems/UIManager.h"
#include "Components/InputManagerComponent.h"
#include "Components/BaseGameComponent.h"
#include "Characters/ShibaCharacter.h"  

// Define log categories
DECLARE_LOG_CATEGORY_EXTERN(LogNaughtyDebug, Log, All);
DEFINE_LOG_CATEGORY(LogNaughtyDebug);

// Simple logging macro
#define NAUGHTY_LOG(Level, Format, ...) UE_LOG(LogNaughtyDebug, Level, Format, ##__VA_ARGS__)

UDebugConsole* UDebugConsole::Instance = nullptr;

UDebugConsole* UDebugConsole::GetInstance(UWorld* World)
{
    if (!Instance)
    {
        Instance = NewObject<UDebugConsole>();
        Instance->AddToRoot(); // Prevent garbage collection
        Instance->InitializeCommands();
        
        NAUGHTY_LOG(Log, TEXT("Debug Console initialized"));
    }
    
    // Always update the cached world reference
    if (World)
    {
        Instance->CachedWorld = World;
    }
    
    return Instance;
}

void UDebugConsole::InitializeCommands()
{
    // Register all available commands
    RegisterCommand(TEXT("teleport"), 
        [this](const TArray<FString>& Args) { HandleTeleportCommand(Args); },
        TEXT("teleport <x> <y> <z> - Teleport player to coordinates"));
        
    RegisterCommand(TEXT("spawn"), 
        [this](const TArray<FString>& Args) { HandleSpawnCommand(Args); },
        TEXT("spawn <item> - Spawn an item at player location"));
        
    RegisterCommand(TEXT("courage"), 
        [this](const TArray<FString>& Args) { HandleCourageCommand(Args); },
        TEXT("courage <amount> - Set player courage level"));
        
    RegisterCommand(TEXT("help"), 
        [this](const TArray<FString>& Args) { HandleHelpCommand(Args); },
        TEXT("help - Display all available commands"));
    
    
    // Core Systems Debug Commands
    RegisterCommand(TEXT("input"), 
        [this](const TArray<FString>& Args) { HandleInputCommand(Args); },
        TEXT("input [test|status] - Test input system or show status"));

    RegisterCommand(TEXT("save"), 
        [this](const TArray<FString>& Args) { HandleSaveCommand(Args); },
        TEXT("save [slot_name] - Save game to specified slot"));

    RegisterCommand(TEXT("load"), 
        [this](const TArray<FString>& Args) { HandleLoadCommand(Args); },
        TEXT("load [slot_name] - Load game from specified slot"));

    RegisterCommand(TEXT("ui"), 
        [this](const TArray<FString>& Args) { HandleUICommand(Args); },
        TEXT("ui [show|hide|list] [layer] - Manage UI layers and widgets"));

    RegisterCommand(TEXT("components"), 
        [this](const TArray<FString>& Args) { HandleComponentsCommand(Args); },
        TEXT("components [list|info] - Show component information"));

    RegisterCommand(TEXT("systems"), 
        [this](const TArray<FString>& Args) { HandleSystemsCommand(Args); },
        TEXT("systems [status|test] - Show systems status or run tests"));
    
    
    // GMCv2-specific commands
    RegisterCommand(TEXT("gamestate"), 
        [this](const TArray<FString>& Args) { HandleGameStateCommand(Args); },
        TEXT("gamestate - Display current game state information"));

    RegisterCommand(TEXT("netinfo"), 
        [this](const TArray<FString>& Args) { HandleNetInfoCommand(Args); },
        TEXT("netinfo - Display network and GMCv2 information"));

    RegisterCommand(TEXT("players"), 
        [this](const TArray<FString>& Args) { HandlePlayersCommand(Args); },
        TEXT("players - List all connected players"));

    RegisterCommand(TEXT("perfmon"), 
        [this](const TArray<FString>& Args) { HandlePerfMonCommand(Args); },
        TEXT("perfmon [on|off] - Toggle performance monitoring display"));

    RegisterCommand(TEXT("timesync"), 
        [this](const TArray<FString>& Args) { HandleTimeSyncCommand(Args); },
        TEXT("timesync - Show time synchronization information"));

    RegisterCommand(TEXT("lagtest"), 
    [this](const TArray<FString>& Args) { HandleLagTestCommand(Args); },
    TEXT("lagtest <ms> - Simulate network lag for testing"));

    RegisterCommand(TEXT("disconnect"), 
        [this](const TArray<FString>& Args) { HandleDisconnectCommand(Args); },
        TEXT("disconnect - Disconnect from server (client only)"));

    RegisterCommand(TEXT("checkinput"), 
    [this](const TArray<FString>& Args) { HandleCheckInputCommand(Args); },
    TEXT("checkinput - Check input setup status"));

    RegisterCommand(TEXT("spawndebug"), 
    [this](const TArray<FString>& Args) { HandleSpawnDebugCommand(Args); },
    TEXT("spawndebug - Check what pawn is actually spawning"));
    
    
    NAUGHTY_LOG(Log, TEXT("Registered %d debug commands"), Commands.Num());
}

void UDebugConsole::HandleSpawnDebugCommand(const TArray<FString>& Args)
{
    // Use the same pattern as HandleInputCommand
    if (!CachedWorld)
    {
        LogError(TEXT("No cached world available"));
        return;
    }

    LogInfo(TEXT("=== SPAWN DEBUG ==="));
    
    // Check GameMode
    AGameModeBase* GameMode = CachedWorld->GetAuthGameMode();
    LogInfo(FString::Printf(TEXT("GameMode: %s"), 
        GameMode ? *GameMode->GetClass()->GetName() : TEXT("NULL")));
    
    if (GameMode)
    {
        LogInfo(FString::Printf(TEXT("DefaultPawnClass: %s"), 
            GameMode->DefaultPawnClass ? *GameMode->DefaultPawnClass->GetName() : TEXT("NULL")));
    }

    // Check PlayerController and Pawn
    APlayerController* PC = CachedWorld->GetFirstPlayerController();
    LogInfo(FString::Printf(TEXT("PlayerController: %s"), 
        PC ? *PC->GetClass()->GetName() : TEXT("NULL")));
    
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;
    LogInfo(FString::Printf(TEXT("Current Pawn: %s"), 
        Pawn ? *Pawn->GetClass()->GetName() : TEXT("NULL")));
    
    if (Pawn)
    {
        // Check for InputManager components
        TArray<UInputManagerComponent*> InputManagers;
        Pawn->GetComponents<UInputManagerComponent>(InputManagers);
        LogInfo(FString::Printf(TEXT("InputManager Components: %d"), InputManagers.Num()));
        
        for (int32 i = 0; i < InputManagers.Num(); i++)
        {
            UInputManagerComponent* InputManager = InputManagers[i];
            LogInfo(FString::Printf(TEXT("  [%d] %s (%s)"), i, 
                *InputManager->GetName(), *InputManager->GetClass()->GetName()));
        }
    }
}

void UDebugConsole::HandleCheckInputCommand(const TArray<FString>& Args)
{
    // Use the same pattern as HandleInputCommand
    if (!CachedWorld)
    {
        LogError(TEXT("No cached world available"));
        return;
    }
    
    APlayerController* PC = CachedWorld->GetFirstPlayerController();
    if (!PC)
    {
        LogError(TEXT("No player controller found"));
        return;
    }

    APawn* CurrentPawn = PC->GetPawn();
    
    LogInfo(TEXT("=== INPUT SETUP CHECK ==="));
    LogInfo(FString::Printf(TEXT("Current Pawn: %s"), CurrentPawn ? *CurrentPawn->GetClass()->GetName() : TEXT("NULL")));
    
    if (CurrentPawn)
    {
        // Check all InputManager components
        TArray<UInputManagerComponent*> InputManagers;
        CurrentPawn->GetComponents<UInputManagerComponent>(InputManagers);
        
        LogInfo(FString::Printf(TEXT("InputManager Components Found: %d"), InputManagers.Num()));
        
        for (int32 i = 0; i < InputManagers.Num(); i++)
        {
            UInputManagerComponent* InputManager = InputManagers[i];
            LogInfo(FString::Printf(TEXT("InputManager %d: %s"), i, *InputManager->GetClass()->GetName()));
        }
    }
}

void UDebugConsole::RegisterCommand(const FString& CommandName, TFunction<void(const TArray<FString>&)> Handler, const FString& Description)
{
    Commands.Add(CommandName.ToLower(), Handler);
    if (!Description.IsEmpty())
    {
        CommandDescriptions.Add(CommandName.ToLower(), Description);
    }
}

void UDebugConsole::ExecuteCommand(const FString& Command)
{
    if (Command.IsEmpty())
    {
        LogWarning(TEXT("Empty command"));
        return;
    }
    
    TArray<FString> ParsedCommand;
    Command.ParseIntoArray(ParsedCommand, TEXT(" "), true);
    
    if (ParsedCommand.Num() == 0)
    {
        LogWarning(TEXT("No command found"));
        return;
    }
    
    FString CommandName = ParsedCommand[0].ToLower();
    ParsedCommand.RemoveAt(0); // Remove command name, leave arguments
    
    if (Commands.Contains(CommandName))
    {
        NAUGHTY_LOG(Log, TEXT("Executing command: %s"), *CommandName);
        Commands[CommandName](ParsedCommand);
    }
    else
    {
        LogError(FString::Printf(TEXT("Unknown command: %s. Type 'help' for available commands."), *CommandName));
    }
}

// Simplified world finding that works in multiplayer
UWorld* GetCurrentGameWorld()
{
    if (!GEngine)
    {
        return nullptr;
    }

    // Check all world contexts and find a game world
    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        UWorld* World = Context.World();
        if (World && (Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE))
        {
            return World;
        }
    }
    
    return nullptr;
}

void UDebugConsole::HandleTeleportCommand(const TArray<FString>& Args)
{
    if (Args.Num() < 3)
    {
        LogError(TEXT("Teleport command requires 3 coordinates: teleport <x> <y> <z>"));
        return;
    }
    
    float X = FCString::Atof(*Args[0]);
    float Y = FCString::Atof(*Args[1]);
    float Z = FCString::Atof(*Args[2]);
    
    LogInfo(FString::Printf(TEXT("Teleport command ready for: %f, %f, %f"), X, Y, Z));
    LogInfo(TEXT("Note: Actual teleportation will be implemented in Section 1D"));
}

void UDebugConsole::HandleSpawnCommand(const TArray<FString>& Args)
{
    if (Args.Num() < 1)
    {
        LogError(TEXT("Spawn command requires item name: spawn <item>"));
        return;
    }
    
    FString ItemName = Args[0];
    LogInfo(FString::Printf(TEXT("Spawn command ready for item: %s"), *ItemName));
    LogInfo(TEXT("Note: Actual spawning will be implemented in later sections"));
}

void UDebugConsole::HandleCourageCommand(const TArray<FString>& Args)
{
    if (Args.Num() < 1)
    {
        LogError(TEXT("Courage command requires amount: courage <amount>"));
        return;
    }
    
    float CourageAmount = FCString::Atof(*Args[0]);
    LogInfo(FString::Printf(TEXT("Courage command ready for amount: %f"), CourageAmount));
    LogInfo(TEXT("Note: Actual courage system will be implemented in Section 1E"));
}

void UDebugConsole::HandleHelpCommand(const TArray<FString>& Args)
{
    LogInfo(TEXT("Available Commands:"));
    for (const auto& CommandPair : CommandDescriptions)
    {
        LogInfo(FString::Printf(TEXT("  %s"), *CommandPair.Value));
    }
}

void UDebugConsole::HandleGameStateCommand(const TArray<FString>& Args)
{
    UWorld* World = GetCurrentGameWorld();
    if (!World)
    {
        LogError(TEXT("No valid game world found"));
        return;
    }
    
    if (ANaughtyGameState* GameState = World->GetGameState<ANaughtyGameState>())
    {
        GameState->LogGameState();
    }
    else if (AGameStateBase* BasicGameState = World->GetGameState())
    {
        LogInfo(TEXT("=== Basic Game State Info ==="));
        LogInfo(FString::Printf(TEXT("Players Connected: %d"), BasicGameState->PlayerArray.Num()));
        LogInfo(FString::Printf(TEXT("Game State Class: %s"), *BasicGameState->GetClass()->GetName()));
    }
    else
    {
        LogWarning(TEXT("No Game State found"));
    }
}

void UDebugConsole::HandleNetInfoCommand(const TArray<FString>& Args)
{
    UWorld* World = GetCurrentGameWorld();
    if (!World)
    {
        LogError(TEXT("No valid game world found"));
        return;
    }
    
    FString NetModeStr;
    switch (World->GetNetMode())
    {
        case NM_Standalone: NetModeStr = TEXT("Standalone"); break;
        case NM_DedicatedServer: NetModeStr = TEXT("Dedicated Server"); break;
        case NM_ListenServer: NetModeStr = TEXT("Listen Server"); break;
        case NM_Client: NetModeStr = TEXT("Client"); break;
        default: NetModeStr = TEXT("Unknown"); break;
    }
    
    LogInfo(FString::Printf(TEXT("Network Mode: %s"), *NetModeStr));
    LogInfo(FString::Printf(TEXT("Has Authority: %s"), World->GetAuthGameMode() ? TEXT("Yes") : TEXT("No")));
    LogInfo(FString::Printf(TEXT("GMCv2 Integration: Active")));
}

void UDebugConsole::HandlePlayersCommand(const TArray<FString>& Args)
{
    UWorld* World = GetCurrentGameWorld();
    if (!World)
    {
        LogError(TEXT("No valid game world found"));
        return;
    }
    
    if (AGameStateBase* GameState = World->GetGameState())
    {
        LogInfo(FString::Printf(TEXT("Connected Players: %d"), GameState->PlayerArray.Num()));
        
        for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
        {
            APlayerState* PlayerState = GameState->PlayerArray[i];
            if (PlayerState)
            {
                FString PlayerName = PlayerState->GetPlayerName();
                if (PlayerName.IsEmpty())
                {
                    PlayerName = FString::Printf(TEXT("Player %d"), i + 1);
                }
                LogInfo(FString::Printf(TEXT("  %d: %s"), i + 1, *PlayerName));
            }
        }
    }
    else
    {
        LogWarning(TEXT("Game State not found"));
    }
}

void UDebugConsole::HandlePerfMonCommand(const TArray<FString>& Args)
{
    if (Args.Num() > 0)
    {
        FString Command = Args[0].ToLower();
        if (Command == TEXT("on"))
        {
            bDebugDisplayEnabled = true;
            LogInfo(TEXT("Performance monitoring enabled"));
        }
        else if (Command == TEXT("off"))
        {
            bDebugDisplayEnabled = false;
            LogInfo(TEXT("Performance monitoring disabled"));
        }
        else
        {
            LogError(TEXT("Usage: perfmon [on|off]"));
        }
    }
    else
    {
        LogInfo(FString::Printf(TEXT("Performance monitoring: %s"), 
                              bDebugDisplayEnabled ? TEXT("On") : TEXT("Off")));
    }
}

void UDebugConsole::HandleTimeSyncCommand(const TArray<FString>& Args)
{
    UWorld* World = GetCurrentGameWorld();
    if (!World)
    {
        LogError(TEXT("No valid game world found"));
        return;
    }
    
    LogInfo(TEXT("=== Time Synchronization ==="));
    LogInfo(FString::Printf(TEXT("World Time: %.3f"), World->GetTimeSeconds()));
    LogInfo(FString::Printf(TEXT("Real Time: %.3f"), World->GetRealTimeSeconds()));
    LogInfo(FString::Printf(TEXT("Delta Time: %.3f"), World->GetDeltaSeconds()));
    
    // Check for GMC WorldTimeReplicator using proper class reference
    TArray<AActor*> WorldTimeReplicators;
    UGameplayStatics::GetAllActorsOfClass(World, AGMC_WorldTimeReplicator::StaticClass(), WorldTimeReplicators);
    
    if (WorldTimeReplicators.Num() > 0)
    {
        LogInfo(FString::Printf(TEXT("GMC WorldTimeReplicator: Found (%d)"), WorldTimeReplicators.Num()));
        
        // Get detailed info from the first replicator
        if (AGMC_WorldTimeReplicator* Replicator = Cast<AGMC_WorldTimeReplicator>(WorldTimeReplicators[0]))
        {
            double ReplicatedTime = Replicator->GetRealWorldTimeSecondsReplicated();
            LogInfo(FString::Printf(TEXT("GMC Replicated Time: %.3f"), ReplicatedTime));
            LogInfo(TEXT("GMC Time Sync: Active"));
        }
    }
    else
    {
        LogWarning(TEXT("GMC WorldTimeReplicator: NOT FOUND"));
        LogInfo(TEXT("GMC Time Sync: Inactive - Add GMC_WorldTimeReplicator to level"));
    }
    
    // Check for GMC Aggregator (we'll need the class name for this too)
    // For now, use string-based detection
    bool bFoundAggregator = false;
    for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
    {
        AActor* Actor = *ActorItr;
        if (Actor && Actor->GetClass()->GetName().Contains(TEXT("GMC_Aggregator")))
        {
            bFoundAggregator = true;
            break;
        }
    }
    
    if (bFoundAggregator)
    {
        LogInfo(TEXT("GMC Aggregator: Found"));
    }
    else
    {
        LogWarning(TEXT("GMC Aggregator: NOT FOUND"));
    }
    
    // Game state info
    if (ANaughtyGameState* GameState = World->GetGameState<ANaughtyGameState>())
    {
        LogInfo(FString::Printf(TEXT("Custom Game State Time: %.3f"), GameState->ServerTime));
    }
    else if (AGameStateBase* BasicGameState = World->GetGameState())
    {
        LogInfo(TEXT("Basic Game State: Active"));
    }
    else
    {
        LogInfo(TEXT("No Game State found"));
    }
}

void UDebugConsole::HandleLagTestCommand(const TArray<FString>& Args)
{
    if (Args.Num() < 1)
    {
        LogError(TEXT("Usage: lagtest <milliseconds>"));
        return;
    }
    
    float LagMS = FCString::Atof(*Args[0]);
    
    // Note: This is a simplified example - real lag simulation requires more complex implementation
    LogInfo(FString::Printf(TEXT("Lag test: %.1f ms simulation requested"), LagMS));
    LogInfo(TEXT("Note: Full lag simulation will be implemented with character movement"));
}

void UDebugConsole::HandleDisconnectCommand(const TArray<FString>& Args)
{
    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        LogError(TEXT("No valid world found"));
        return;
    }
    
    if (World->GetAuthGameMode() != nullptr)
    {
        LogWarning(TEXT("Cannot disconnect - this is the server"));
        return;
    }
    
    LogInfo(TEXT("Disconnecting from server..."));
    
    // Get the first player controller and disconnect
    if (APlayerController* PC = World->GetFirstPlayerController())
    {
        PC->ConsoleCommand(TEXT("disconnect"));
    }
}

void UDebugConsole::HandleInputCommand(const TArray<FString>& Args)
{
    if (Args.Num() == 0)
    {
        LogInfo(TEXT("Usage: input [test|status]"));
        return;
    }

    FString Command = Args[0].ToLower();
    
    if (Command == TEXT("test"))
    {
        FString TestMessage = TEXT("Input system test initiated\n");
        TestMessage += TEXT("Press any keys to test input - use 'input status' to check");
        LogInfo(TestMessage);
    }
    else if (Command == TEXT("status"))
    {
        // Use cached world instead of GEngine->GetWorldFromContextObject
        if (CachedWorld && CachedWorld->GetFirstPlayerController())
        {
            APawn* PlayerPawn = CachedWorld->GetFirstPlayerController()->GetPawn();
            if (PlayerPawn)
            {
                UInputManagerComponent* InputManager = PlayerPawn->FindComponentByClass<UInputManagerComponent>();
                if (InputManager)
                {
                    FString StatusMessage = TEXT("=== Input System Status ===\n");
                    StatusMessage += FString::Printf(TEXT("Sprint Pressed: %s\n"), InputManager->IsSprintPressed() ? TEXT("Yes") : TEXT("No"));
                    StatusMessage += FString::Printf(TEXT("Crouch Pressed: %s\n"), InputManager->IsCrouchPressed() ? TEXT("Yes") : TEXT("No"));
                    StatusMessage += FString::Printf(TEXT("Jump Pressed: %s\n"), InputManager->IsJumpPressed() ? TEXT("Yes") : TEXT("No"));
                    StatusMessage += FString::Printf(TEXT("Pick Up Pressed: %s\n"), InputManager->IsPickUpPressed() ? TEXT("Yes") : TEXT("No"));
                    StatusMessage += FString::Printf(TEXT("Sniff Vision Active: %s\n"), InputManager->IsSniffVisionActive() ? TEXT("Yes") : TEXT("No"));
                    StatusMessage += FString::Printf(TEXT("Howl Pressed: %s\n"), InputManager->IsHowlPressed() ? TEXT("Yes") : TEXT("No"));
                    StatusMessage += FString::Printf(TEXT("Move Input: %.2f, %.2f\n"), InputManager->GetMoveInput().X, InputManager->GetMoveInput().Y);
                    StatusMessage += FString::Printf(TEXT("Look Input: %.2f, %.2f"), InputManager->GetLookInput().X, InputManager->GetLookInput().Y);
                    
                    LogInfo(StatusMessage);
                }
                else
                {
                    LogWarning(TEXT("Input Manager Component not found on player pawn"));
                }
            }
            else
            {
                LogWarning(TEXT("No player pawn found"));
            }
        }
        else if (CachedWorld)
        {
            LogWarning(TEXT("No player controller found"));
        }
        else
        {
            LogError(TEXT("No cached world available"));
        }
    }
    else
    {
        LogError(TEXT("Unknown input command. Use: test, status"));
    }
}

void UDebugConsole::HandleSaveCommand(const TArray<FString>& Args)
{
    if (CachedWorld)
    {
        if (UGameInstance* GameInstance = CachedWorld->GetGameInstance())
        {
            if (USaveSystemManager* SaveManager = GameInstance->GetSubsystem<USaveSystemManager>())
            {
                FString SlotName = TEXT("DefaultSave");
                if (Args.Num() > 0)
                {
                    SlotName = Args[0];
                }
                
                SaveManager->SaveGame(SlotName, 0);
                LogInfo(FString::Printf(TEXT("Save initiated for slot: %s"), *SlotName));
            }
            else
            {
                LogError(TEXT("Save System Manager not found"));
            }
        }
        else
        {
            LogError(TEXT("Game Instance not found"));
        }
    }
    else
    {
        LogError(TEXT("No world context available"));
    }
}

void UDebugConsole::HandleLoadCommand(const TArray<FString>& Args)
{
    if (CachedWorld)
    {
        if (UGameInstance* GameInstance = CachedWorld->GetGameInstance())
        {
            if (USaveSystemManager* SaveManager = GameInstance->GetSubsystem<USaveSystemManager>())
            {
                FString SlotName = TEXT("DefaultSave");
                if (Args.Num() > 0)
                {
                    SlotName = Args[0];
                }
                
                bool bSuccess = SaveManager->LoadGame(SlotName, 0);
                if (bSuccess)
                {
                    LogInfo(FString::Printf(TEXT("Game loaded successfully from slot: %s"), *SlotName));
                }
                else
                {
                    LogError(FString::Printf(TEXT("Failed to load game from slot: %s"), *SlotName));
                }
            }
            else
            {
                LogError(TEXT("Save System Manager not found"));
            }
        }
        else
        {
            LogError(TEXT("Game Instance not found"));
        }
    }
    else
    {
        LogError(TEXT("No world context available"));
    }
}

void UDebugConsole::HandleUICommand(const TArray<FString>& Args)
{
    if (Args.Num() == 0)
    {
        LogInfo(TEXT("Usage: ui [show|hide|list] [layer]"));
        return;
    }

    FString Command = Args[0].ToLower();
    
    if (CachedWorld)
    {
        if (UGameInstance* GameInstance = CachedWorld->GetGameInstance())
        {
            if (UUIManager* UIManager = GameInstance->GetSubsystem<UUIManager>())
            {
                if (Command == TEXT("list"))
                {
                    // Build the UI status in proper ascending order
                    FString UIStatus = TEXT("=== UI System Status ===\n");
                    UIStatus += FString::Printf(TEXT("Background Layer Widgets: %d\n"), UIManager->GetWidgetCount(EUILayer::Background));
                    UIStatus += FString::Printf(TEXT("Game Layer Widgets: %d\n"), UIManager->GetWidgetCount(EUILayer::Game));
                    UIStatus += FString::Printf(TEXT("Menu Layer Widgets: %d\n"), UIManager->GetWidgetCount(EUILayer::Menu));
                    UIStatus += FString::Printf(TEXT("Popup Layer Widgets: %d\n"), UIManager->GetWidgetCount(EUILayer::Popup));
                    UIStatus += FString::Printf(TEXT("Modal Layer Widgets: %d\n"), UIManager->GetWidgetCount(EUILayer::Modal));
                    UIStatus += FString::Printf(TEXT("System Layer Widgets: %d"), UIManager->GetWidgetCount(EUILayer::System));
                    
                    LogInfo(UIStatus);
                }
                else if (Command == TEXT("show"))
                {
                    if (Args.Num() > 1)
                    {
                        FString LayerName = Args[1].ToLower();
                        EUILayer Layer = EUILayer::Game;
                        
                        if (LayerName == TEXT("background")) Layer = EUILayer::Background;
                        else if (LayerName == TEXT("game")) Layer = EUILayer::Game;
                        else if (LayerName == TEXT("menu")) Layer = EUILayer::Menu;
                        else if (LayerName == TEXT("popup")) Layer = EUILayer::Popup;
                        else if (LayerName == TEXT("modal")) Layer = EUILayer::Modal;
                        else if (LayerName == TEXT("system")) Layer = EUILayer::System;
                        
                        UIManager->SetLayerVisibility(Layer, true);
                        LogInfo(FString::Printf(TEXT("Showed UI layer: %s"), *LayerName));
                    }
                    else
                    {
                        LogError(TEXT("Layer name required. Options: background, game, menu, popup, modal, system"));
                    }
                }
                else if (Command == TEXT("hide"))
                {
                    if (Args.Num() > 1)
                    {
                        FString LayerName = Args[1].ToLower();
                        EUILayer Layer = EUILayer::Game;
                        
                        if (LayerName == TEXT("background")) Layer = EUILayer::Background;
                        else if (LayerName == TEXT("game")) Layer = EUILayer::Game;
                        else if (LayerName == TEXT("menu")) Layer = EUILayer::Menu;
                        else if (LayerName == TEXT("popup")) Layer = EUILayer::Popup;
                        else if (LayerName == TEXT("modal")) Layer = EUILayer::Modal;
                        else if (LayerName == TEXT("system")) Layer = EUILayer::System;
                        
                        UIManager->SetLayerVisibility(Layer, false);
                        LogInfo(FString::Printf(TEXT("Hid UI layer: %s"), *LayerName));
                    }
                    else
                    {
                        LogError(TEXT("Layer name required. Options: background, game, menu, popup, modal, system"));
                    }
                }
                else
                {
                    LogError(TEXT("Unknown UI command. Use: show, hide, list"));
                }
            }
            else
            {
                LogError(TEXT("UI Manager not found"));
            }
        }
        else
        {
            LogError(TEXT("Game Instance not found"));
        }
    }
    else
    {
        LogError(TEXT("No world context available"));
    }
}

void UDebugConsole::HandleComponentsCommand(const TArray<FString>& Args)
{
    // Use cached world instead of GEngine->GetWorldFromContextObject
    if (!CachedWorld || !CachedWorld->GetFirstPlayerController())
    {
        LogError(TEXT("No valid world or player controller found"));
        return;
    }

    APawn* PlayerPawn = CachedWorld->GetFirstPlayerController()->GetPawn();
    if (!PlayerPawn)
    {
        LogWarning(TEXT("No player pawn found"));
        return;
    }

    if (Args.Num() == 0 || Args[0].ToLower() == TEXT("list"))
    {
        FString ComponentList = TEXT("=== Player Pawn Components ===\n");
        TArray<UActorComponent*> Components = PlayerPawn->GetComponents().Array();
        
        for (UActorComponent* Component : Components)
        {
            if (Component)
            {
                FString ComponentInfo = FString::Printf(TEXT("- %s (%s)"), 
                    *Component->GetName(), 
                    *Component->GetClass()->GetName());
                    
                if (UBaseGameComponent* BaseComponent = Cast<UBaseGameComponent>(Component))
                {
                    ComponentInfo += FString::Printf(TEXT(" [Init: %s, Active: %s]"),
                        BaseComponent->IsComponentInitialized() ? TEXT("Yes") : TEXT("No"),
                        BaseComponent->IsComponentActive() ? TEXT("Yes") : TEXT("No"));
                }
                
                ComponentList += ComponentInfo + TEXT("\n");
            }
        }
        
        LogInfo(ComponentList);
    }
    else if (Args[0].ToLower() == TEXT("info"))
    {
        FString DetailedInfo = TEXT("=== Detailed Component Information ===\n");
        TArray<UBaseGameComponent*> BaseComponents;
        PlayerPawn->GetComponents<UBaseGameComponent>(BaseComponents);
        
        for (UBaseGameComponent* Component : BaseComponents)
        {
            if (Component)
            {
                DetailedInfo += Component->GetComponentDebugString() + TEXT("\n");
            }
        }
        
        LogInfo(DetailedInfo);
    }
    else
    {
        LogError(TEXT("Unknown components command. Use: list, info"));
    }
}

void UDebugConsole::HandleSystemsCommand(const TArray<FString>& Args)
{
    if (Args.Num() == 0 || Args[0].ToLower() == TEXT("status"))
    {
        // Build the entire status as a single string and log it once
        FString StatusMessage = TEXT("=== Core Systems Status ===\n");
        
        if (CachedWorld)
        {
            if (UGameInstance* GameInstance = CachedWorld->GetGameInstance())
            {
                if (USaveSystemManager* SaveManager = GameInstance->GetSubsystem<USaveSystemManager>())
                {
                    StatusMessage += FString::Printf(TEXT("Save System: Active (Auto-save: %s)\n"), 
                        SaveManager->IsAutoSaveEnabled() ? TEXT("On") : TEXT("Off"));
                    
                    if (UNaughtySaveGame* SaveData = SaveManager->GetCurrentSaveData())
                    {
                        StatusMessage += FString::Printf(TEXT("  Current Save: %.1f hours played\n"), SaveData->GetPlayTimeHours());
                    }
                }
                else
                {
                    StatusMessage += TEXT("Save System: NOT FOUND\n");
                }
                
                if (UUIManager* UIManager = GameInstance->GetSubsystem<UUIManager>())
                {
                    int32 TotalWidgets = UIManager->GetAllWidgets().Num();
                    StatusMessage += FString::Printf(TEXT("UI System: Active (%d widgets)\n"), TotalWidgets);
                }
                else
                {
                    StatusMessage += TEXT("UI System: NOT FOUND\n");
                }
            }
            
            // Input System
            if (CachedWorld->GetFirstPlayerController())
            {
                APawn* PlayerPawn = CachedWorld->GetFirstPlayerController()->GetPawn();
                if (PlayerPawn && PlayerPawn->FindComponentByClass<UInputManagerComponent>())
                {
                    StatusMessage += TEXT("Input System: Active");
                }
                else
                {
                    StatusMessage += TEXT("Input System: Component not found");
                }
            }
            else
            {
                StatusMessage += TEXT("Input System: No player controller");
            }
        }
        else
        {
            StatusMessage += TEXT("ERROR: No world context available");
        }
        
        // Use existing LogInfo method
        LogInfo(StatusMessage);
    }
    else if (Args[0].ToLower() == TEXT("test"))
    {
        // ... rest of method stays the same
    }
    else
    {
        LogError(TEXT("Unknown systems command. Use: status, test"));
    }
}

void UDebugConsole::LogInfo(const FString& Message)
{
    NAUGHTY_LOG(Log, TEXT("%s"), *Message);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
    }
}

void UDebugConsole::LogWarning(const FString& Message)
{
    NAUGHTY_LOG(Warning, TEXT("%s"), *Message);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
    }
}

void UDebugConsole::LogError(const FString& Message)
{
    NAUGHTY_LOG(Error, TEXT("%s"), *Message);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
    }
}

void UDebugConsole::StartPerformanceTimer(const FString& TimerName)
{
    PerformanceTimers.Add(TimerName, FPlatformTime::Seconds());
    NAUGHTY_LOG(VeryVerbose, TEXT("Started timer: %s"), *TimerName);
}

void UDebugConsole::EndPerformanceTimer(const FString& TimerName)
{
    if (PerformanceTimers.Contains(TimerName))
    {
        double StartTime = PerformanceTimers[TimerName];
        double ElapsedTime = (FPlatformTime::Seconds() - StartTime) * 1000.0; // Convert to milliseconds
        
        LogInfo(FString::Printf(TEXT("Timer '%s': %.2f ms"), *TimerName, ElapsedTime));
        PerformanceTimers.Remove(TimerName);
    }
    else
    {
        LogWarning(FString::Printf(TEXT("Timer '%s' not found"), *TimerName));
    }
}

void UDebugConsole::ToggleDebugDisplay()
{
    bDebugDisplayEnabled = !bDebugDisplayEnabled;
    LogInfo(FString::Printf(TEXT("Debug display: %s"), bDebugDisplayEnabled ? TEXT("Enabled") : TEXT("Disabled")));
}

void UDebugConsole::DrawDebugSphere(const FVector& Location, float Radius, const FColor& Color)
{
    if (!bDebugDisplayEnabled)
    {
        return;
    }
    
    UWorld* World = GetCurrentGameWorld();
    if (World)
    {
        ::DrawDebugSphere(World, Location, Radius, 12, Color, false, 0.1f);
    }
}

void UDebugConsole::DrawDebugCircle(const FVector& Location, float Radius, const FColor& Color)
{
    if (!bDebugDisplayEnabled)
    {
        return;
    }
    
    UWorld* World = GetCurrentGameWorld();
    if (World)
    {
        ::DrawDebugCircle(World, Location, Radius, 32, Color, false, 0.1f, 0, 2.0f, FVector(1,0,0), FVector(0,1,0));
    }
}


// Console command to initialize and use debug console
static FAutoConsoleCommand DebugConsoleCommand(
    TEXT("naughty.debug"),
    TEXT("Execute debug console command. Usage: naughty.debug <command>"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        // Use a simple approach - just get any game world
        UWorld* World = GetCurrentGameWorld();
        if (!World)
        {
            UE_LOG(LogTemp, Error, TEXT("Naughty Debug: No valid world found for console command"));
            return;
        }
        
        UDebugConsole* Console = UDebugConsole::GetInstance(World);
        if (!Console)
        {
            UE_LOG(LogTemp, Error, TEXT("Naughty Debug: Failed to create debug console instance"));
            return;
        }
        
        if (Args.Num() > 0)
        {
            FString Command = FString::Join(Args, TEXT(" "));
            Console->ExecuteCommand(Command);
        }
        else
        {
            Console->LogInfo(TEXT("Debug console ready! Try: naughty.debug help"));
        }
    })
);