#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NaughtyGameState.generated.h"

/**
 * Game State for Naughty Shiba
 * Manages global game state and multiplayer synchronization
 */
UCLASS()
class NAUGHTYSHIBA_API ANaughtyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANaughtyGameState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Global game state variables (replicated)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	float ServerTime = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	int32 TotalPlayersConnected = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bGameInProgress = false;

	// Debug functionality
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void LogGameState();

protected:
	// Network replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// Debug console reference
	UPROPERTY()
	class UDebugConsole* DebugConsole;
};