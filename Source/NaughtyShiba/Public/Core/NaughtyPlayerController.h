#pragma once

#include "CoreMinimal.h"
#include "GMCPlayerController.h"
#include "NaughtyPlayerController.generated.h"

/**
 * Player Controller for Naughty Shiba using GMCv2
 * Handles time synchronization and player input management
 */
UCLASS()
class NAUGHTYSHIBA_API ANaughtyPlayerController : public AGMC_PlayerController
{
	GENERATED_BODY()

public:
	ANaughtyPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Input handling
	virtual void SetupInputComponent() override;

	// Debug console integration
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void OpenDebugConsole();

private:
	// Debug console reference
	UPROPERTY()
	class UDebugConsole* DebugConsole;
};