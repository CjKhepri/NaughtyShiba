#include "Systems/DebugConsole.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Define log category once
DEFINE_LOG_CATEGORY(LogNaughtyDebug);

// Simple logging macro
#define NAUGHTY_LOG(Level, Format, ...) UE_LOG(LogNaughtyDebug, Level, Format, ##__VA_ARGS__)

UDebugConsole* UDebugConsole::Instance = nullptr;

UDebugConsole* UDebugConsole::GetInstance(UWorld* World)
{
    if (!Instance)
    {
        Instance = NewObject<UDebugConsole>();
        Instance->InitializeCommands();
        Instance->AddToRoot(); // Prevent garbage collection
        
        NAUGHTY_LOG(Log, TEXT("Debug Console initialized"));
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

    NAUGHTY_LOG(Log, TEXT("Registered %d debug commands"), Commands.Num());
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
    
    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
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
    
    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
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
        if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWorld())
        {
            UWorld* World = GEngine->GameViewport->GetWorld();
            UDebugConsole* Console = UDebugConsole::GetInstance(World);
            
            if (Args.Num() > 0)
            {
                FString Command = FString::Join(Args, TEXT(" "));
                Console->ExecuteCommand(Command);
            }
            else
            {
                Console->LogInfo(TEXT("Debug console ready! Try: naughty.debug help"));
            }
        }
    })
);