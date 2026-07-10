#include "classes.h"
#include <vendor/SDK/Engine_classes.hpp>

namespace Meccha
{
    SDK::APlayerController* Player::GetPlayerController()
    {
        auto World = SDK::UWorld::GetWorld();
        if (!World)
            return nullptr;

        auto GameInstance = World->OwningGameInstance;
        if (!GameInstance)
            return nullptr;

        if (GameInstance->LocalPlayers.Num() == 0)
            return nullptr;

        auto LocalPlayer = GameInstance->LocalPlayers[0];
        if (!LocalPlayer)
            return nullptr;

        return LocalPlayer->PlayerController;
    }

    SDK::ACharacter* Player::GetCharacter()
    {
        auto localPawn = GetPawn();
        return static_cast<SDK::ACharacter*>(localPawn);
    }

    SDK::APawn* Player::GetPawn()
    {
        auto PC = GetPlayerController();
        if (!PC)
            return nullptr;
        return PC->AcknowledgedPawn;
    }

    bool Player::IsHunter(SDK::ACharacter* character)
    {
        if (!character) 
            return false;
        return character->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Hunter_C::StaticClass());
    }

    SDK::UWorld* World::GetWorld()
    {
        return SDK::UWorld::GetWorld();
    }

    std::vector<SDK::ACharacter*> World::GetAllPlayers()
    {
        std::vector<SDK::ACharacter*> players;

        auto World = SDK::UWorld::GetWorld();
        if (!World)
            return players;

        auto GameState = World->GameState;
        if (!GameState)
            return players;

        for (auto PlayerState : GameState->PlayerArray)
        {
            if (!PlayerState)
                continue;

            auto Pawn = PlayerState->PawnPrivate;
            if (!Pawn)
                continue;

            auto Character = static_cast<SDK::ACharacter*>(Pawn);
            if (Character)
                players.push_back(Character);
        }

        return players;
    }

    static SDK::UWorld* World = nullptr;
    static std::vector<PlayerInfo> PlayerData;
    static bool bValid = false;

    const std::vector<PlayerInfo>& PlayerManager::GetPlayers()
    {
        SDK::UWorld* currentWorld = SDK::UWorld::GetWorld();

        if (currentWorld != World || !bValid)
        {
            World = currentWorld;
            PlayerData.clear();

            if (currentWorld && currentWorld->GameState)
            {
                auto& PlayerArray = currentWorld->GameState->PlayerArray;
                PlayerData.reserve(PlayerArray.Num());

                for (auto PlayerState : PlayerArray)
                {
                    if (!PlayerState)
                        continue;

                    SDK::APawn* Pawn = PlayerState->PawnPrivate;
                    SDK::ACharacter* Char = Pawn ? static_cast<SDK::ACharacter*>(Pawn) : nullptr;

                    PlayerInfo info;

                    info.PlayerState = PlayerState;
                    info.Name = PlayerState->GetPlayerName();
                    info.Location = Char ? Char->K2_GetActorLocation() : SDK::FVector();
                    info.Character = Char;
                    info.Pawn = Pawn;

                    if (Char && Char->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_Hunter_C::StaticClass()))
                        info.bIsHunter = true;
                    else
                        info.bIsHunter = false;

                    PlayerData.push_back(info);
                }
            }

            bValid = true;
        }

        return PlayerData;
    }

    void PlayerManager::Invalidate()
    {
        bValid = false;
    }
}