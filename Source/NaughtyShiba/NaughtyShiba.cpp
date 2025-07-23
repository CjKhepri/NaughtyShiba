#include "NaughtyShiba.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogNaughtyShiba);

void FNaughtyShibaModule::StartupModule()
{
	// This code will execute after your module is loaded into memory
	UE_LOG(LogNaughtyShiba, Warning, TEXT("NaughtyShiba module has started"));
}

void FNaughtyShibaModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module
	UE_LOG(LogNaughtyShiba, Warning, TEXT("NaughtyShiba module has shut down"));
}

IMPLEMENT_MODULE(FNaughtyShibaModule, NaughtyShiba)