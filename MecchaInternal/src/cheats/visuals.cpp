#define NOMINMAX
#include <Windows.h>

#include "cheats.h"
#include "../globals.h"

#include <vendor/imgui/framework/framework.h>
#include <vendor/SDK/Engine_classes.hpp>
#include <algorithm>
#include <cstdio>
#include <cstdint>

using namespace Globals;

namespace Meccha::Cheats
{
    void ESP::Draw()
    {
        auto localPC = Player::GetPlayerController();
        if (!localPC)
            return;

        auto localPawn = Player::GetPawn();
        if (!localPawn)
            return;

        const auto& players = PlayerManager::GetPlayers();
        if (players.empty())
            return;

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;

        for (const auto& p : players)
        {
            if (!p.Character || p.Character == localPawn || !p.Pawn)
                continue;

            SDK::FVector origin, boxExtent;
            p.Character->GetActorBounds(false, &origin, &boxExtent, false);

            SDK::FVector feetLocation = origin - SDK::FVector(0, 0, boxExtent.Z);
            SDK::FVector headLocation = origin + SDK::FVector(0, 0, boxExtent.Z);

            SDK::FVector2D feetPos{};
            SDK::FVector2D headPos{};

            bool feetOnScreen = localPC->ProjectWorldLocationToScreen(feetLocation, &feetPos, true);
            bool headOnScreen = localPC->ProjectWorldLocationToScreen(headLocation, &headPos, true);

            if (!feetOnScreen && !headOnScreen)
                continue;

            if (!headOnScreen)
                headPos = feetPos;
            if (!feetOnScreen)
                feetPos = headPos;

            float boxHeight = feetPos.Y - headPos.Y;
            if (boxHeight < 10.0f)
                continue;

            float boxWidth = boxHeight * 0.60f; // max box multiplier

            float left = feetPos.X - boxWidth * 0.5f;
            float right = feetPos.X + boxWidth * 0.5f;

            float offsetY = 10.0f;
            float top = headPos.Y + offsetY;
            float bottom = feetPos.Y + offsetY;

            if (bottom < 0 || top > screenSize.y || right < 0 || left > screenSize.x)
                continue;

            ImU32 color = Features::ESP::boxColor;
            ImU32 fillColor = Features::ESP::fillColor;
            ImU32 nameColor = Features::ESP::nameColor;
            ImU32 outlineColor = Features::ESP::outlineColor;

            if (Features::ESP::teams && p.bIsHunter)
            {
                color = Features::ESP::hunterColor;
                fillColor = color;
                nameColor = color;
            }

            if (Features::ESP::filled)
            {
                constexpr float FillAlpha = 0.2f;

                ImU32 finalFill = (fillColor & 0x00FFFFFF) | (static_cast<ImU32>(FillAlpha * 255.0f) << 24);

                Framework::Draw::FilledRect(draw, ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertU32ToFloat4(finalFill));
            }

            if (Features::ESP::box)
            {
                if (Features::ESP::cornered)
                {
                    float cornerSize = std::min(boxWidth, boxHeight) * 0.17f; // corner size multiplier
                    if (cornerSize < 10.0f) cornerSize = 10.0f;
                    if (Features::ESP::outline)
                        Framework::Draw::CorneredRect(draw, ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertU32ToFloat4(color), ImGui::ColorConvertU32ToFloat4(outlineColor), cornerSize, 2.0f, 3.0f);
                    else
                        Framework::Draw::CorneredRect(draw, ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertU32ToFloat4(color), ImGui::ColorConvertU32ToFloat4(outlineColor), cornerSize, 2.0f, 0.0f);
                }
                else
                {
                    if (Features::ESP::outline)
                        Framework::Draw::OutlinedRect(draw, ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertU32ToFloat4(color), ImGui::ColorConvertU32ToFloat4(outlineColor), 0.0f, 1.5f, 3.0f);
                    else
                        Framework::Draw::Rect(draw, ImVec2(left, top), ImVec2(right, bottom), ImGui::ColorConvertU32ToFloat4(color), 0.0f, 1.5f);
                }
            }

            if (Features::ESP::chams) { }

            if (Features::ESP::names)
            {
                std::string name = p.Name.ToString();
                ImVec2 nameSize = ImGui::CalcTextSize(name.c_str());
                ImVec2 namePos(left + (right - left - nameSize.x) * 0.5f, top - nameSize.y - 2.0f);
                if (namePos.y > 0)
                {
                    if (Features::ESP::outline)
                        Framework::Draw::TextOutlined(draw, namePos, name.c_str(), ImGui::ColorConvertU32ToFloat4(nameColor), ImGui::ColorConvertU32ToFloat4(outlineColor), 1.0f, false);
                    else
                        Framework::Draw::Text(draw, namePos, name.c_str(), ImGui::ColorConvertU32ToFloat4(nameColor), false);
                }
            }

            if (Features::ESP::distance)
            {
                float distUnits = p.Location.GetDistanceTo(localPawn->K2_GetActorLocation());
                float distMeters = distUnits / 100.0f;
                char distText[32];
                sprintf_s(distText, "%.1fm", distMeters);
                ImVec2 distSize = ImGui::CalcTextSize(distText);
                ImVec2 distPos(left + (right - left - distSize.x) * 0.5f, bottom + 2.0f);
                if (distPos.y + distSize.y < screenSize.y)
                {
                    if (Features::ESP::outline)
                        Framework::Draw::TextOutlined(draw, distPos, distText, ImGui::ColorConvertU32ToFloat4(nameColor), ImGui::ColorConvertU32ToFloat4(outlineColor), 1.0f, false);
                    else
                        Framework::Draw::Text(draw, distPos, distText, ImGui::ColorConvertU32ToFloat4(nameColor), false);
                }
            }

            if (Features::ESP::skeleton)
            { 
                if (!p.Pawn->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_C::StaticClass()))
                    continue;
                
                auto character = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(p.Pawn); 

                auto* Mesh = character->Mesh; // is now safe to use
                if (!Mesh || !Mesh->SkeletalMesh || !Mesh->SkeletalMesh->Skeleton) continue; 

                std::int32_t numBones = Mesh->GetNumBones(); 
                if (numBones <= 0) continue;

                int boneIndices[] = {
                    6,  5,  2,  3,  4,  8,  9,  10, 11,
                    13, 14, 15, 16, 18, 19, 20, 21,
                    23, 24, 25, 26
                };

                int boneCount = 21;
                SDK::FVector bonePos[21];
                bool valid[21] = { false };

                for (int j = 0; j < boneCount; ++j)
                {
                    if (boneIndices[j] >= numBones) continue;
                    SDK::FName boneName = Mesh->GetBoneName(boneIndices[j]);
                    if (boneName.IsNone()) continue;
                    SDK::FTransform worldTransform = Mesh->GetBoneTransform(boneName, SDK::ERelativeTransformSpace::RTS_World);
                    SDK::FVector pos = worldTransform.Translation;
                    if (pos.X != 0 || pos.Y != 0 || pos.Z != 0)
                    {
                        bonePos[j] = pos;
                        valid[j] = true;
                    }
                }

                auto DrawBoneLine = [&](int from, int to) 
                    {
                        if (!valid[from] || !valid[to]) return;
                        SDK::FVector2D screenFrom, screenTo;
                        bool fromScreen = localPC->ProjectWorldLocationToScreen(bonePos[from], &screenFrom, true);
                        bool toScreen = localPC->ProjectWorldLocationToScreen(bonePos[to], &screenTo, true);
                        if (fromScreen && toScreen)
                        {
                            ImVec2 p1(screenFrom.X, screenFrom.Y);
                            ImVec2 p2(screenTo.X, screenTo.Y);
                            if (Features::ESP::outline)
                                Framework::Draw::OutlinedLine(draw, p1, p2, ImGui::ColorConvertU32ToFloat4(color), ImGui::ColorConvertU32ToFloat4(outlineColor), 1.0f, 0.65f);
                            else
                                Framework::Draw::Line(draw, p1, p2, ImGui::ColorConvertU32ToFloat4(color), 1.0f);
                        }
                    };

                DrawBoneLine(0, 1);
                DrawBoneLine(1, 2);
                DrawBoneLine(2, 3);
                DrawBoneLine(3, 4);
                DrawBoneLine(1, 5);
                DrawBoneLine(5, 6);
                DrawBoneLine(6, 7);
                DrawBoneLine(7, 8);
                DrawBoneLine(1, 9);
                DrawBoneLine(9, 10);
                DrawBoneLine(10, 11);
                DrawBoneLine(11, 12);
                DrawBoneLine(4, 13);
                DrawBoneLine(13, 14);
                DrawBoneLine(14, 15);
                DrawBoneLine(15, 16);
                DrawBoneLine(4, 17);
                DrawBoneLine(17, 18);
                DrawBoneLine(18, 19);
                DrawBoneLine(19, 20);
            }
        }
    }
}