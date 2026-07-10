#include "../framework.h"

namespace Framework::Widgets
{
    void MenuPlayerEntry(const char* label, const Meccha::PlayerInfo& player, bool& selected, const std::function<void(bool)>& onToggle, const char* prefix, const ImVec4& prefixColor, float height)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiID id = window->GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 size(avail.x, height);

        ImGui::PushID(id);
        ImGui::Selectable("##item", false, ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick, size);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();
        ImGui::PopID();

        if (clicked)
        {
            selected = !selected;
            if (onToggle)
                onToggle(selected);
        }

        float dt = ImGui::GetIO().DeltaTime;
        AnimData& anim = g_AnimCache[id];

        ImVec4 targetBgColor;
        if (active)
            targetBgColor = ImVec4(0.14f, 0.14f, 0.14f, 0.9f);
        else if (hovered)
            targetBgColor = ImVec4(0.01f, 0.01f, 0.01f, 0.25f);
        else if (selected)
            targetBgColor = ImVec4(0.10f, 0.10f, 0.11f, 0.40f);
        else
            targetBgColor = ImVec4(0.10f, 0.10f, 0.11f, 0.00f);

        const float bgSpeed = 12.0f;
        const float barSpeed = 12.0f;

        anim.currentBgColor.x += (targetBgColor.x - anim.currentBgColor.x) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.y += (targetBgColor.y - anim.currentBgColor.y) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.z += (targetBgColor.z - anim.currentBgColor.z) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.w += (targetBgColor.w - anim.currentBgColor.w) * ImMin(1.0f, dt * bgSpeed);

        float targetBarWidth = selected ? 3.0f : 0.0f;
        anim.currentBarWidth += (targetBarWidth - anim.currentBarWidth) * ImMin(1.0f, dt * barSpeed);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (anim.currentBgColor.w > 0.001f)
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(anim.currentBgColor));

        if (anim.currentBarWidth > 0.001f)
            dl->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + anim.currentBarWidth, pos.y + size.y), ImGui::GetColorU32(Colors::Accent));

        ImVec2 nameSize = ImGui::CalcTextSize(label);
        ImVec2 prefixSize = prefix ? ImGui::CalcTextSize(prefix) : ImVec2(0.0f, 0.0f);
        float spaceWidth = prefix ? ImGui::CalcTextSize(" ").x : 0.0f;

        float totalTextWidth = nameSize.x + (prefix ? (spaceWidth + prefixSize.x) : 0.0f);

        float startX = pos.x + (size.x - totalTextWidth) * 0.5f;
        float textY = pos.y + (size.y - nameSize.y) * 0.5f;

        ImVec4 textColor = selected ? Colors::Text : Colors::TextSecondary;

        dl->AddText(ImVec2(startX, textY), ImGui::GetColorU32(textColor), label);
        startX += nameSize.x;

        if (prefix)
        {
            dl->AddText(ImVec2(startX, textY), ImGui::GetColorU32(textColor), " ");
            startX += spaceWidth;

            dl->AddText(ImVec2(startX, textY), ImGui::GetColorU32(prefixColor), prefix);
        }
    }

    void PlayerInfoWidget(const Meccha::PlayerInfo& player, float width, float offsetY)
    {
        const float titleBarHeight = 28.0f;

        ImVec2 pos = ImVec2(g_MainMenuPos.x + g_MainMenuSize.x + 15.0f, g_MainMenuPos.y + offsetY);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0), ImVec2(width, FLT_MAX));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));

        std::string title = std::string(ICON_FA_USER) + " " + player.Name.ToString();
        ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSize = ImGui::GetWindowSize();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + titleBarHeight), ImGui::GetColorU32(Colors::Accent));

        ImVec2 titleSize = ImGui::CalcTextSize(title.c_str());
        ImVec2 titlePos(winPos.x + (winSize.x - titleSize.x) * 0.5f, winPos.y + (titleBarHeight - titleSize.y) * 0.5f);
        dl->AddText(titlePos, ImGui::GetColorU32(Colors::White), title.c_str());

        ImGui::SetCursorPosY(titleBarHeight);

        ImGui::Text("Role: %s", player.bIsHunter ? "Hunter" : "Hider");
        ImGui::Text("Location: (%.1f, %.1f, %.1f)", player.Location.X, player.Location.Y, player.Location.Z);

        if (player.Character)
        {
            SDK::APlayerController* PC = static_cast<SDK::APlayerController*>(player.Character->Controller);

            if (ImGui::Button("Spectate"))
            {
                auto localPC = Meccha::Player::GetPlayerController();
                if (localPC)
                {
                    localPC->PlayerState->bIsSpectator = true;

                    SDK::FViewTargetTransitionParams params;
                    params.BlendTime = 2.f;
                    params.BlendFunction = SDK::EViewTargetBlendFunction::VTBlend_EaseIn;// smooth animation

                    localPC->ClientSetViewTarget(player.PlayerState->GetPawn(), params);
                }
            }
            ImGui::SameLine();

            if (ImGui::Button("Stop spectating"))
            {
                auto localPC = Meccha::Player::GetPlayerController();
                if (localPC)
                {
                    localPC->PlayerState->bIsSpectator = false;

                    auto myPawn = Meccha::Player::GetPawn();
                    SDK::FViewTargetTransitionParams params;
                    params.BlendTime = 2.f;
                    params.BlendFunction = SDK::EViewTargetBlendFunction::VTBlend_EaseOut; // smooth animation

                    localPC->ClientSetViewTarget(myPawn, params);
                }
            }

            if (ImGui::Button("Teleport Random"))
            {
                SDK::FVector currentLoc = player.Character->K2_GetActorLocation();
                SDK::FVector randomOffset(rand() % 1000 - 500, rand() % 1000 - 500, rand() % 200 + 50);
                SDK::FVector destLoc = currentLoc + randomOffset;
                SDK::FRotator destRot(rand() % 360, rand() % 360, rand() % 360);
                player.Character->K2_TeleportTo(destLoc, destRot);
            }

            ImGui::SameLine();

            if (ImGui::Button("Teleport to"))
            {
                auto pawn = Meccha::Player::GetPawn();
                if (pawn) {
                    SDK::FVector NewLoc = player.Location;
                    SDK::FRotator NewRot(0, 0, 0);
                    pawn->K2_TeleportTo(NewLoc, NewRot);
                }
            }

            if (ImGui::Button("Disable input"))
            {
                if (PC)
                    player.Character->DisableInput(PC);
            }
            ImGui::SameLine();

            if (ImGui::Button("Enable input"))
            {
                if (PC)
                    player.Character->EnableInput(PC);
            }

            if (ImGui::Button("Kick"))
            {
                if (PC)
                    PC->ClientReturnToMainMenuWithTextReason(SDK::FText()); // however the fuck you create an fstring idfk
            }

            ImGui::SameLine();

            if (ImGui::Button("Lag player"))
            {
                if (PC)
                    PC->ClientCapBandwidth(10);
            }

            ImGui::Separator();

            // god this code is bad. 

            static std::vector<std::pair<std::string, std::string>> SoundList;
            static int CurrentSoundIndex = 0;
            static bool bSoundListLoaded = false;
            static float VolumeMultiplier = 1.0f;
            static float PitchMultiplier = 1.0f;

            if (!bSoundListLoaded)
            {
                SoundList.clear();
                uintptr_t GObjectsAddress = SDK::InSDKUtils::GetImageBase() + SDK::Offsets::GObjects;
                SDK::TUObjectArray* GObjects = reinterpret_cast<SDK::TUObjectArray*>(GObjectsAddress);
                if (GObjects)
                {
                    int32_t num = GObjects->Num();
                    for (int32_t i = 0; i < num; ++i)
                    {
                        SDK::UObject* Obj = GObjects->GetByIndex(i);
                        if (!Obj) continue;
                        if (Obj->IsA(SDK::USoundBase::StaticClass()))
                        {
                            std::string name = Obj->GetName();
                            std::string fullName = Obj->GetFullName();
                            if (!name.empty() && name != "Default__SoundBase" && name != "Default__SoundWave" && name != "Default__SoundCue")
                                SoundList.push_back({ name, fullName });
                        }
                    }
                }
                bSoundListLoaded = true;
            }

            if (ImGui::Button("Play Sound"))
            {
                if (PC && !SoundList.empty() && CurrentSoundIndex >= 0 && CurrentSoundIndex < (int)SoundList.size())
                {
                    const std::string& fullName = SoundList[CurrentSoundIndex].second;
                    SDK::USoundBase* Sound = SDK::UObject::FindObject<SDK::USoundBase>(fullName);
                    if (Sound && Sound->IsA(SDK::USoundBase::StaticClass()))
                        PC->ClientPlaySound(Sound, VolumeMultiplier, PitchMultiplier);
                }
            }

            ImGui::SameLine();

            if (ImGui::BeginCombo("Select sound", SoundList.empty() ? "No sounds" : SoundList[CurrentSoundIndex].first.c_str()))
            {
                for (int i = 0; i < (int)SoundList.size(); ++i)
                {
                    bool isSelected = (CurrentSoundIndex == i);
                    if (ImGui::Selectable(SoundList[i].first.c_str(), isSelected))
                        CurrentSoundIndex = i;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            //ImGui::Separator();

            ImGui::SetNextItemWidth(100.0f);
            ImGui::DragFloat("##Vol", &VolumeMultiplier, 0.01f, 0.0f, 10.0f, "Vol %.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            ImGui::DragFloat("##Pitch", &PitchMultiplier, 0.01f, 0.0f, 10.0f, "Pitch %.2f");

            ImGui::Separator();
        }
        else
        {
            ImGui::TextDisabled("Character is null (not in game?)");
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
    }
}