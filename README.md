Naughty Shiba 🐕
A multiplayer dog simulation game built with Unreal Engine 5.3 and GMCv2 for networking.
🎮 Game Overview
Play as a mischievous Shiba Inu dog in a multiplayer world where you can explore, interact with the environment, and cause adorable chaos with friends.
🏗️ Technical Architecture
Core Technologies

Engine: Unreal Engine 5.3
Networking: GMCv2 (General Movement Component v2)
Input: Enhanced Input System
Language: C++ with Blueprint integration

Current Features

✅ Character Controller - Full movement with GMC networking
✅ 4 Core Abilities - Jump, Bark, Howl, Sniff Vision
✅ Multiplayer Ready - Tested with Listen Server + Client
✅ Animation System - C++ base class with Blueprint implementation
✅ Input System - Enhanced Input with component-based architecture
✅ Core Systems - Save/Load, UI Manager, Debug Console

Development Status

Phase 1 Section 1D: 90% Complete (Animation implementation)
Working Abilities: 4/6 implemented and network-tested
Next Milestone: Complete animation system and advance to Section 1E

🛠️ Development Setup
Prerequisites

Unreal Engine 5.3
Visual Studio 2022
Git LFS (for large files)

Getting Started

Clone the repository
Right-click NaughtyShiba.uproject → "Generate Visual Studio project files"
Open NaughtyShiba.sln in Visual Studio
Build solution (Development Editor configuration)
Launch from Visual Studio or open .uproject file

Project Structure
Source/
├── NaughtyShiba/
│   ├── Public/
│   │   ├── Characters/        # Character classes
│   │   ├── Components/        # Reusable components
│   │   ├── Animation/         # Animation system
│   │   ├── Movement/          # GMC movement
│   │   ├── Systems/           # Core game systems
│   │   └── Core/              # Game framework
│   └── Private/               # Implementation files
Content/
├── Blueprints/
│   ├── Characters/            # Character blueprints
│   └── UI/                    # User interface
└── Maps/                      # Game levels
🎯 Roadmap
Phase 1: Foundation (Current)

 Section 1A: Project Setup & Technical Foundation
 Section 1B: GMCv2 Integration & Multiplayer Framework
 Section 1C: Core Systems Architecture
 Section 1D: Animation System (90% complete)
 Section 1E: Core Ability Systems

Phase 2: Advanced Systems

Territory Management System
Advanced AI and Social Mechanics
Complex Physics Interactions
Advanced Multiplayer Features

🤝 Contributing
This project is in active development. Current focus is on completing the animation system and core abilities.
Development Workflow

Create feature branches from main
Test thoroughly in both single-player and multiplayer
Ensure all abilities work with GMC networking
Submit pull requests with detailed descriptions

📋 Current Development Focus
Animation System Implementation

Setting up locomotion state machines
Creating blend spaces for movement
Implementing ability animations (Bark, Howl, Jump, Sniff)
Testing multiplayer animation replication

Known Working Systems

✅ Character Movement - GMC networking confirmed
✅ Input Handling - Enhanced Input with InputManager
✅ Abilities - Jump, Bark, Howl, Sniff Vision (networked)
✅ State Management - Character states with proper transitions
✅ Debug Systems - Console commands and real-time logging

🐛 Debugging
Debug Console Commands
Access via backtick () key or naughty.debug <command>`:

help - Show all available commands
input status - Check input system status
gamestate - Display current game state
netinfo - Show networking information
players - List connected players

Common Issues

Blueprint Corruption: Recreate BP_ShibaCharacter if input stops working
Network Issues: Verify GMC components are properly configured
Animation Issues: Check debug logging for real-time animation values

📄 License
[Your chosen license]

Built with ❤️ and lots of dog treats 🦴
