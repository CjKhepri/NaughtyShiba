#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NaughtyGameInstance.generated.h"

/**
 * Custom Game Instance for Naughty Shiba
 * Manages subsystems and global game state
 */
UCLASS()
class NAUGHTYSHIBA_API UNaughtyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UNaughtyGameInstance();

protected:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void OnStart() override;

public:
	// Subsystem access helpers
	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	class USaveSystemManager* GetSaveSystemManager() const;

	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	class UUIManager* GetUIManager() const;

	// Game state management
	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	void InitializeGameSystems();

	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	void ShutdownGameSystems();

	// Debug functionality
	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	void LogSystemsStatus();

protected:
	// System initialization flags
	bool bSystemsInitialized = false;

private:
	// Debug console reference
	UPROPERTY()
	class UDebugConsole* DebugConsole;
};