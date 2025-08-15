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
            "StructUtils",             // Required for GMCv2
            "GMCCore",                 // GMCv2 main module
            "ToolMenus",               // UI framework support
            "DeveloperSettings"        // For save system settings
        });

        // Modules we don't want to expose in headers
        PrivateDependencyModuleNames.AddRange(new string[] { 
            "EngineSettings",          // Engine configuration access
            "AudioMixer"               // Audio system support
        });

        // SUPPRESS COMMON BUILD WARNINGS (NEW SECTION)
        PublicDefinitions.AddRange(new string[]
        {
            // Enhanced Input deprecation warnings
            "UE_DISABLE_DEPRECATED_INPUT_WARNINGS=1",
            "SUPPRESS_ENHANCED_INPUT_DEPRECATION_WARNINGS=1",
            
            // GMCv2 plugin warnings
            "SUPPRESS_MONOLITHIC_HEADER_WARNINGS=1",
            "SUPPRESS_TEMPLATE_INSTANTIATION_WARNINGS=1",
            
            // General UE5 warnings
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING=1",
            "_CRT_SECURE_NO_WARNINGS=1"
        });

        // Platform-specific warning suppressions
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.AddRange(new string[]
            {
                "WIN32_LEAN_AND_MEAN=1",
                "NOMINMAX=1"
            });
        }

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
        
        // Enable save system features
        PublicDefinitions.Add("WITH_SAVE_SYSTEM=1");

        // Additional build optimizations for cleaner compilation
        bLegacyPublicIncludePaths = false;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        bUseUnity = true;
    }
}