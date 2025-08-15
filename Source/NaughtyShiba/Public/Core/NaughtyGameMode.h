#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NaughtyGameMode.generated.h"

/**
 * Game Mode for Naughty Shiba
 * Handles multiplayer session management and GMCv2 integration
 */
UCLASS()
class NAUGHTYSHIBA_API ANaughtyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANaughtyGameMode();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when a player joins
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Called when a player leaves
	virtual void Logout(AController* Exiting) override;

public:
	// Multiplayer configuration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Multiplayer")
	int32 MaxPlayers = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Multiplayer")
	bool bAllowSpectators = true;

	// Debug functionality
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void LogServerInfo();

private:
	// Debug console reference
	UPROPERTY()
	class UDebugConsole* DebugConsole;

	// Connected players count
	int32 ConnectedPlayers = 0;
};