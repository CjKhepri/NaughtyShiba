#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// Main log category for the game
DECLARE_LOG_CATEGORY_EXTERN(LogNaughtyShiba, Log, All);

class FNaughtyShibaModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

// Development helper macros
#if NAUGHTY_DEBUG
#define NAUGHTY_LOG(Level, Format, ...) UE_LOG(LogNaughtyShiba, Level, Format, ##__VA_ARGS__)
#define NAUGHTY_LOG_FUNC() UE_LOG(LogNaughtyShiba, VeryVerbose, TEXT("Entering %s"), *FString(__FUNCTION__))
#define NAUGHTY_LOG_SCREEN(Format, ...) \
if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(Format, ##__VA_ARGS__)); }
#else
#define NAUGHTY_LOG(Level, Format, ...)
#define NAUGHTY_LOG_FUNC()
#define NAUGHTY_LOG_SCREEN(Format, ...)
#endif

// Object validation helpers
#define NAUGHTY_VALIDATE(Object) \
if (!IsValid(Object)) \
{ \
NAUGHTY_LOG(Error, TEXT("Invalid object in %s: %s"), *FString(__FUNCTION__), TEXT(#Object)); \
return; \
}

#define NAUGHTY_VALIDATE_RETURN(Object, ReturnValue) \
if (!IsValid(Object)) \
{ \
NAUGHTY_LOG(Error, TEXT("Invalid object in %s: %s"), *FString(__FUNCTION__), TEXT(#Object)); \
return ReturnValue; \
}

// Network role checking (for GMCv2 integration)
#define NAUGHTY_CHECK_AUTHORITY() \
if (!HasAuthority()) \
{ \
NAUGHTY_LOG(Warning, TEXT("Function %s called without authority"), *FString(__FUNCTION__)); \
return; \
}