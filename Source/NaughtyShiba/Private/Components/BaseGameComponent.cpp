#include "Components/BaseGameComponent.h"
#include "Systems/DebugConsole.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UBaseGameComponent::UBaseGameComponent()
{
    // Set this component to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false; // Enable after initialization

    // Initialize state
    bIsInitialized = false;
    bIsActive = false;
    DebugConsole = nullptr;
}

void UBaseGameComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize debug console
    if (GetWorld())
    {
        DebugConsole = UDebugConsole::GetInstance(GetWorld());
    }

    LogDebug(FString::Printf(TEXT("Component BeginPlay: %s"), *GetClass()->GetName()));

    // Check network authority if required
    if (bRequiresNetworkAuthority && !HasAuthority())
    {
        LogWarning(TEXT("Component requires network authority but doesn't have it"));
        return;
    }

    // Auto-initialize if enabled
    if (bAutoInitialize)
    {
        if (InitializationDelay > 0.0f)
        {
            // Delayed initialization
            if (UWorld* World = GetWorld())
            {
                World->GetTimerManager().SetTimer(InitializationTimerHandle, this, 
                    &UBaseGameComponent::DelayedInitialization, InitializationDelay, false);
            }
        }
        else
        {
            // Immediate initialization
            InitializeComponent();
        }
    }
}

void UBaseGameComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    LogDebug(FString::Printf(TEXT("Component EndPlay: %s"), *GetClass()->GetName()));

    // Clean up timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(InitializationTimerHandle);
    }

    // Deactivate component
    if (bIsActive)
    {
        DeactivateComponent();
    }

    Super::EndPlay(EndPlayReason);
}

void UBaseGameComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Only tick if component is active
    if (!bIsActive)
    {
        return;
    }

    // Override in derived classes for per-frame logic
}

void UBaseGameComponent::InitializeComponent()
{
    if (bIsInitialized)
    {
        LogWarning(TEXT("Component already initialized"));
        return;
    }

    // Check required components
    if (!CheckRequiredComponents())
    {
        LogError(TEXT("Required components not found - initialization failed"));
        return;
    }

    bIsInitialized = true;
    LogDebug(FString::Printf(TEXT("Component initialized: %s"), *GetClass()->GetName()));

    // Call Blueprint event
    OnComponentInitialized();
    OnComponentInitializedEvent.Broadcast(this);

    // Auto-activate if enabled
    if (bAutoActivateComponent)
    {
        ActivateComponent();
    }
}

void UBaseGameComponent::ActivateComponent()
{
    if (!bIsInitialized)
    {
        LogWarning(TEXT("Cannot activate - component not initialized"));
        return;
    }

    if (bIsActive)
    {
        LogWarning(TEXT("Component already active"));
        return;
    }

    bIsActive = true;
    SetComponentTickEnabled(true);

    LogDebug(FString::Printf(TEXT("Component activated: %s"), *GetClass()->GetName()));

    OnComponentActivatedEvent.Broadcast(this);
}

void UBaseGameComponent::DeactivateComponent()
{
    if (!bIsActive)
    {
        return;
    }

    bIsActive = false;
    SetComponentTickEnabled(false);

    LogDebug(FString::Printf(TEXT("Component deactivated: %s"), *GetClass()->GetName()));

    OnComponentDeactivatedEvent.Broadcast(this);
}

bool UBaseGameComponent::HasAuthority() const
{
    if (AActor* Owner = GetOwner())
    {
        return Owner->HasAuthority();
    }
    return false;
}

bool UBaseGameComponent::IsLocallyControlled() const
{
    if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
    {
        return PawnOwner->IsLocallyControlled();
    }
    return false;
}

void UBaseGameComponent::LogComponentInfo()
{
    FString InfoString = GetComponentDebugString();
    LogDebug(InfoString);
}

FString UBaseGameComponent::GetComponentDebugString() const
{
    return FString::Printf(TEXT("Component: %s | Owner: %s | Initialized: %s | Active: %s | Authority: %s"),
        *GetClass()->GetName(),
        GetOwner() ? *GetOwner()->GetName() : TEXT("None"),
        bIsInitialized ? TEXT("Yes") : TEXT("No"),
        bIsActive ? TEXT("Yes") : TEXT("No"),
        HasAuthority() ? TEXT("Yes") : TEXT("No"));
}

bool UBaseGameComponent::CheckRequiredComponents() const
{
    if (!GetOwner())
    {
        return false;
    }

    for (TSubclassOf<UActorComponent> RequiredClass : RequiredComponents)
    {
        if (!RequiredClass)
        {
            continue;
        }

        UActorComponent* FoundComponent = GetOwner()->FindComponentByClass(RequiredClass);
        if (!FoundComponent)
        {
            LogError(FString::Printf(TEXT("Required component not found: %s"), *RequiredClass->GetName()));
            return false;
        }
    }

    return true;
}

void UBaseGameComponent::LogDebug(const FString& Message) const
{
    if (bDebugMode && DebugConsole)
    {
        FString FormattedMessage = FString::Printf(TEXT("[%s] %s"), *GetClass()->GetName(), *Message);
        DebugConsole->LogInfo(FormattedMessage);
    }
}

void UBaseGameComponent::LogWarning(const FString& Message) const
{
    if (DebugConsole)
    {
        FString FormattedMessage = FString::Printf(TEXT("[%s] %s"), *GetClass()->GetName(), *Message);
        DebugConsole->LogWarning(FormattedMessage);
    }
}

void UBaseGameComponent::LogError(const FString& Message) const
{
    if (DebugConsole)
    {
        FString FormattedMessage = FString::Printf(TEXT("[%s] %s"), *GetClass()->GetName(), *Message);
        DebugConsole->LogError(FormattedMessage);
    }
}

void UBaseGameComponent::DelayedInitialization()
{
    InitializeComponent();
}