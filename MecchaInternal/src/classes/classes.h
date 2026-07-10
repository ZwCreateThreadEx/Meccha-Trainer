#pragma once
#include <cstdint>

#include <vendor/SDK.hpp>
#include <vendor/SDK/CoreUObject_structs.hpp>
#include <vendor/SDK/Engine_structs.hpp>
#include <vector>

namespace Meccha
{
    class Player
    {
    public:
        static SDK::APlayerController* GetPlayerController();
        static SDK::ACharacter* GetCharacter();
        static SDK::APawn* GetPawn();
        static bool IsHunter(SDK::ACharacter* character);
    };

    struct PlayerInfo
    {
        SDK::APlayerState* PlayerState;
        SDK::FString Name;
        SDK::FVector Location;
        SDK::ACharacter* Character;
        SDK::APawn* Pawn;
        bool bIsHunter;
    };

    class PlayerManager
    {
    public:
        static const std::vector<PlayerInfo>& GetPlayers();
        static void Invalidate();
    };

    class World
    {
    public:
        static SDK::UWorld* GetWorld();
        static std::vector<SDK::ACharacter*> GetAllPlayers();
    };

    class Camera final
    {
    };
}