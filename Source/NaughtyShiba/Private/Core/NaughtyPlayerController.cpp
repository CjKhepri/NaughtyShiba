#include "Core/NaughtyPlayerController.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"

ANaughtyPlayerController::ANaughtyPlayerController()
{
	// Set this controller to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
    
	// Initialize debug console reference
	DebugConsole = nullptr;
}

void ANaughtyPlayerController::BeginPlay()
{
	Super::BeginPlay();
    
	// Initialize debug console for this player
	if (GetWorld())
	{
		DebugConsole = UDebugConsole::GetInstance(GetWorld());
		if (DebugConsole)
		{

		}
	}
}

void ANaughtyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	// GMCv2 handles time synchronization automatically
	// Add any per-frame logic here if needed
}

void ANaughtyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
    
	// Enhanced Input will be configured in Section 1D
	// For now, set up basic console access
	if (InputComponent)
	{
		// Bind console key (backtick) to open debug console
		InputComponent->BindAction("OpenConsole", IE_Pressed, this, &ANaughtyPlayerController::OpenDebugConsole);
	}
}

void ANaughtyPlayerController::OpenDebugConsole()
{
	if (DebugConsole)
	{
		DebugConsole->LogInfo(TEXT("Debug console opened via Player Controller"));
	}
}