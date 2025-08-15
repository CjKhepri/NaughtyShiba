#include "Core/NaughtyGameState.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

ANaughtyGameState::ANaughtyGameState()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
    
	// Enable replication
	bReplicates = true;
    
	// Initialize debug console reference
	DebugConsole = nullptr;
}

void ANaughtyGameState::BeginPlay()
{
	Super::BeginPlay();
    
	// Initialize debug console
	if (GetWorld())
	{
		DebugConsole = UDebugConsole::GetInstance(GetWorld());
		if (DebugConsole)
		{
			DebugConsole->LogInfo(TEXT("Naughty Game State initialized"));
		}
	}
    
	// Start game progression
	bGameInProgress = true;
}

void ANaughtyGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	// Update server time (only on server)
	if (HasAuthority())
	{
		ServerTime += DeltaTime;
		TotalPlayersConnected = PlayerArray.Num();
	}
}

void ANaughtyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	// Replicate game state variables
	DOREPLIFETIME(ANaughtyGameState, ServerTime);
	DOREPLIFETIME(ANaughtyGameState, TotalPlayersConnected);
	DOREPLIFETIME(ANaughtyGameState, bGameInProgress);
}

void ANaughtyGameState::LogGameState()
{
	if (DebugConsole)
	{
		DebugConsole->LogInfo(FString::Printf(TEXT("Game State - Time: %.2f, Players: %d, In Progress: %s"), 
											ServerTime, TotalPlayersConnected, 
											bGameInProgress ? TEXT("Yes") : TEXT("No")));
	}
}