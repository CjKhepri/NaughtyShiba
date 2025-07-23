using UnrealBuildTool;

public class NaughtyShiba : ModuleRules
{
    public NaughtyShiba(ReadOnlyTargetRules Target) : base(Target)
    {
        // Use explicit PCH for faster compilation
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Core modules required for basic functionality
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core",                    // Essential UE core
            "CoreUObject",             // UObject system
            "Engine",                  // Main engine functionality
            "InputCore",               // Input handling
            "HeadMountedDisplay",      // VR support (future-proofing)
            "UMG",                     // UI system
            "Slate",                   // Low-level UI
            "SlateCore",               // UI core
            "OnlineSubsystem",         // Multiplayer foundation
            "OnlineSubsystemUtils",    // Multiplayer utilities
            "EnhancedInput",           // Modern input system
            "StructUtils"              // Required for GMCv2
        });

        // Modules we don't want to expose in headers
        PrivateDependencyModuleNames.AddRange(new string[] { 
            // GMCv2 will be added in Section 1B
        });

        // Conditional compilation flags
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            // Enable debug features in development builds
            PublicDefinitions.Add("NAUGHTY_DEBUG=1");
        }
        else
        {
            // Disable debug features in shipping builds
            PublicDefinitions.Add("NAUGHTY_DEBUG=0");
        }

        // Enable enhanced input system
        PublicDefinitions.Add("WITH_ENHANCED_INPUT=1");
    }
}