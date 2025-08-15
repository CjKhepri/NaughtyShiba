#include "Core/NaughtyGameMode.h"
#include "Characters/ShibaCharacter.h"
#include "Core/NaughtyPlayerController.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ANaughtyGameMode::ANaughtyGameMode()
{
    // Set default classes for GMCv2
    PlayerControllerClass = ANaughtyPlayerController::StaticClass();
    
    // Use the CORRECT Blueprint CLASS path (note the _C suffix)
    static ConstructorHelpers::FClassFinder<APawn> ShibaCharacterBP(TEXT("/Game/Blueprints/Characters/BP_ShibaCharacter.BP_ShibaCharacter_C"));
    if (ShibaCharacterBP.Class != nullptr)
    {
        DefaultPawnClass = ShibaCharacterBP.Class;
        UE_LOG(LogTemp, Warning, TEXT("BP_ShibaCharacter loaded successfully!"));
    }
    else
    {
        // Fallback to C++ class if Blueprint not found
        UE_LOG(LogTemp, Error, TEXT("BP_ShibaCharacter not found! Using C++ fallback"));
        DefaultPawnClass = AShibaCharacter::StaticClass();
    }
    
    // Configure multiplayer settings
    bUseSeamlessTravel = true;
    
    // Initialize debug console reference
    DebugConsole = nullptr;
}
void ANaughtyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize debug console for server
    if (GetWorld())
    {
        DebugConsole = UDebugConsole::GetInstance(GetWorld());
        if (DebugConsole)
        {
            /*LogServerInfo();*/
        }
    }
}

void ANaughtyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    
    ConnectedPlayers++;
    
    /*if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Player joined. Connected players: %d/%d"), 
                                            ConnectedPlayers, MaxPlayers));
    }*/
    
    // Notify the new player's controller
    if (ANaughtyPlayerController* NaughtyPC = Cast<ANaughtyPlayerController>(NewPlayer))
    {
        // Additional setup for Naughty player controller if needed
    }
}

void ANaughtyGameMode::Logout(AController* Exiting)
{
    if (Exiting && Exiting->IsA<APlayerController>())
    {
        ConnectedPlayers--;
        
        if (DebugConsole)
        {
            DebugConsole->LogInfo(FString::Printf(TEXT("Player left. Connected players: %d/%d"), 
                                                ConnectedPlayers, MaxPlayers));
        }
    }
    
    Super::Logout(Exiting);
}

void ANaughtyGameMode::LogServerInfo()
{
    if (DebugConsole)
    {
        DebugConsole->LogInfo(FString::Printf(TEXT("Server Info - Max Players: %d, Spectators Allowed: %s"), 
                                            MaxPlayers, bAllowSpectators ? TEXT("Yes") : TEXT("No")));
    }
}