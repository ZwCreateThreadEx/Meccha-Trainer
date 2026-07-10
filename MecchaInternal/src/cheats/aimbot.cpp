#define NOMINMAX
#include <Windows.h>
#include <excpt.h>

#include <vendor/SimpleMath/SimpleMath.h>
#include <vendor/SDK/Engine_classes.hpp>
#include <algorithm>
#include <cmath>

#include "cheats.h"
#include "../globals.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;
using namespace Globals;

namespace Meccha::Cheats
{
    static int ImGuiKeyToVK(int imguiKey)
    {
        if (imguiKey >= ImGuiKey_MouseLeft && imguiKey <= ImGuiKey_MouseX2)
            return VK_LBUTTON + (imguiKey - ImGuiKey_MouseLeft);
        if (imguiKey >= ImGuiKey_A && imguiKey <= ImGuiKey_Z)
            return 'A' + (imguiKey - ImGuiKey_A);
        if (imguiKey >= ImGuiKey_0 && imguiKey <= ImGuiKey_9)
            return '0' + (imguiKey - ImGuiKey_0);
        if (imguiKey >= ImGuiKey_F1 && imguiKey <= ImGuiKey_F24)
            return VK_F1 + (imguiKey - ImGuiKey_F1);
        if (imguiKey >= ImGuiKey_Keypad0 && imguiKey <= ImGuiKey_Keypad9)
            return VK_NUMPAD0 + (imguiKey - ImGuiKey_Keypad0);
        switch (imguiKey) {
        case ImGuiKey_LeftShift: return VK_LSHIFT;
        case ImGuiKey_RightShift: return VK_RSHIFT;
        case ImGuiKey_LeftCtrl: return VK_LCONTROL;
        case ImGuiKey_RightCtrl: return VK_RCONTROL;
        case ImGuiKey_LeftAlt: return VK_LMENU;
        case ImGuiKey_RightAlt: return VK_RMENU;
        case ImGuiKey_Space: return VK_SPACE;
        case ImGuiKey_Enter: return VK_RETURN;
        case ImGuiKey_Escape: return VK_ESCAPE;
        case ImGuiKey_Tab: return VK_TAB;
        case ImGuiKey_Backspace: return VK_BACK;
        case ImGuiKey_Insert: return VK_INSERT;
        case ImGuiKey_Delete: return VK_DELETE;
        case ImGuiKey_Home: return VK_HOME;
        case ImGuiKey_End: return VK_END;
        case ImGuiKey_PageUp: return VK_PRIOR;
        case ImGuiKey_PageDown: return VK_NEXT;
        case ImGuiKey_LeftArrow: return VK_LEFT;
        case ImGuiKey_RightArrow: return VK_RIGHT;
        case ImGuiKey_UpArrow: return VK_UP;
        case ImGuiKey_DownArrow: return VK_DOWN;
        case ImGuiKey_KeypadDecimal: return VK_DECIMAL;
        case ImGuiKey_KeypadDivide: return VK_DIVIDE;
        case ImGuiKey_KeypadMultiply: return VK_MULTIPLY;
        case ImGuiKey_KeypadSubtract: return VK_SUBTRACT;
        case ImGuiKey_KeypadAdd: return VK_ADD;
        case ImGuiKey_KeypadEnter: return VK_RETURN;
        case ImGuiKey_GraveAccent: return VK_OEM_3;
        case ImGuiKey_Minus: return VK_OEM_MINUS;
        case ImGuiKey_Equal: return VK_OEM_PLUS;
        case ImGuiKey_LeftBracket: return VK_OEM_4;
        case ImGuiKey_RightBracket: return VK_OEM_6;
        case ImGuiKey_Backslash: return VK_OEM_5;
        case ImGuiKey_Semicolon: return VK_OEM_1;
        case ImGuiKey_Apostrophe: return VK_OEM_7;
        case ImGuiKey_Comma: return VK_OEM_COMMA;
        case ImGuiKey_Period: return VK_OEM_PERIOD;
        case ImGuiKey_Slash: return VK_OEM_2;
        default: return 0;
        }
    }

    static SDK::FVector GetBoneWorldLocation(SDK::APawn* Pawn, int boneIndex)
    {
        if (!Pawn || !Pawn->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_C::StaticClass())) return SDK::FVector(0, 0, 0);
        

        auto character = static_cast<SDK::ABP_FirstPersonCharacter_cLeon_Character_C*>(Pawn);
        if (!character) return SDK::FVector(0, 0, 0);

        auto* Mesh = character->Mesh;
        if (!Mesh || !Mesh->SkeletalMesh) return SDK::FVector(0, 0, 0);

        std::int32_t numBones = Mesh->GetNumBones();
        if (boneIndex < 0 || boneIndex >= numBones) return SDK::FVector(0, 0, 0);

        SDK::FName boneName = Mesh->GetBoneName(boneIndex);
        if (boneName.IsNone()) return SDK::FVector(0, 0, 0);

        SDK::FTransform worldTransform = Mesh->GetBoneTransform(boneName, SDK::ERelativeTransformSpace::RTS_World);
        SDK::FVector pos = worldTransform.Translation;

        //if (pos.X == 0.0f && pos.Y == 0.0f && pos.Z == 0.0f)
        //    return SDK::FVector(0, 0, 0);

        return pos;
    }

    static Vector3 SDKVectorToDX(const SDK::FVector& v)
    {
        return Vector3(static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z));
    }

    void Aimbot::Run()
    {
        if (Features::Aimbot::fovCircle)
        {
            const auto& io = ImGui::GetIO();
            ImDrawList* draw = ImGui::GetForegroundDrawList();
            ImVec2 center = io.DisplaySize;
            center.x *= 0.5f;
            center.y *= 0.5f;
            float radius = (Features::Aimbot::fov / 90.0f) * (io.DisplaySize.x * 0.5f);
            if (radius > 0.0f) // just disable fov circle yo
            {
                ImU32 outlineColor = IM_COL32(0, 0, 0, 200);
                ImU32 mainColor = IM_COL32(255, 255, 255, 200);
                if (Features::ESP::outline)
                    draw->AddCircle(center, radius, outlineColor, 64, 4.0f);
                draw->AddCircle(center, radius, mainColor, 64, 2.0f);
            }
        }

        int vk = ImGuiKeyToVK(static_cast<int>(Features::Aimbot::aimKey));
        if (vk == 0) return;

        bool shouldAim = false;
        if (Features::Aimbot::aimMode)
            shouldAim = (GetAsyncKeyState(vk) & 0x8000) != 0;
        else
        {
            if (GetAsyncKeyState(vk) & 1)
                Features::Aimbot::aimActive = !Features::Aimbot::aimActive;
            shouldAim = Features::Aimbot::aimActive;
        }

        if (!shouldAim) return;

        auto localPC = Player::GetPlayerController();
        if (!localPC) return;

        auto localPawn = Player::GetPawn();
        if (!localPawn) return;

        auto localChar = Player::GetCharacter();
        if (!localChar) return;

        if (Features::Aimbot::hunterOnly && !Meccha::Player::IsHunter(localChar))
            return;

        auto camMgr = localPC->PlayerCameraManager;
        if (!camMgr) return;

        SDK::FCameraCacheEntry cameraCache = camMgr->CameraCachePrivate;
        Vector3 cameraPos = SDKVectorToDX(cameraCache.POV.Location);
        SDK::FRotator cameraRot = cameraCache.POV.Rotation;

        float pitchRad = cameraRot.Pitch * XM_PI / 180.0f;
        float yawRad = cameraRot.Yaw * XM_PI / 180.0f;
        Vector3 forward(cosf(yawRad) * cosf(pitchRad),
            sinf(yawRad) * cosf(pitchRad),
            sinf(pitchRad));

        const auto& players = PlayerManager::GetPlayers();
        if (players.empty()) return;

        float bestAngle = std::numeric_limits<float>::max();
        Vector3 bestTarget{ };

        for (const auto& p : players)
        {
            if (!p.Character || !p.Pawn || !p.Pawn->IsA(SDK::ABP_FirstPersonCharacter_cLeon_Character_C::StaticClass())) continue;
            if (p.Character == localPawn) continue;
            if (p.Character->Controller == localPC) continue; 

            SDK::FVector targetPosSDK = GetBoneWorldLocation(p.Pawn, 6); // index 6 head bone
            if (targetPosSDK.X == 0.0f && targetPosSDK.Y == 0.0f && targetPosSDK.Z == 0.0f)
                continue;

            Vector3 targetPos = SDKVectorToDX(targetPosSDK);
            Vector3 delta = targetPos - cameraPos;
            float len = delta.Length();
            if (len < 1.0f) continue;

            Vector3 dir = delta / len;
            float dot = forward.Dot(dir);
            if (dot < 0.0f) continue;

            float angle = acosf(dot);
            if (angle < bestAngle)
            {
                bestAngle = angle;
                bestTarget = targetPos;
            }
        }

        float fovRad = Features::Aimbot::fov * XM_PI / 180.0f;
        if (bestAngle > fovRad)
            return;

        Vector3 delta = bestTarget - cameraPos;
        float len = delta.Length();
        if (len < 1.0f) return;

        Vector3 dir = delta / len;

        float targetPitch = asin(dir.z) * 180.0f / XM_PI;
        float targetYaw = atan2(dir.y, dir.x) * 180.0f / XM_PI;

        float deltaPitch = targetPitch - cameraRot.Pitch;
        float deltaYaw = targetYaw - cameraRot.Yaw;

        auto NormalizeAngle = [](float angle) {
            angle = fmod(angle + 180.0f, 360.0f);
            if (angle < 0) angle += 360.0f;
            return angle - 180.0f;
            };

        deltaPitch = NormalizeAngle(deltaPitch);
        deltaYaw = NormalizeAngle(deltaYaw);

        float smooth = Features::Aimbot::smoothness;
        if (smooth > 0.0f)
        {
            float alpha = 1.0f / (smooth + 0.1f);
            deltaPitch *= alpha;
            deltaYaw *= alpha;
        }

        float newPitch = cameraRot.Pitch + deltaPitch;
        float newYaw = cameraRot.Yaw + deltaYaw;
        newPitch = std::clamp(newPitch, -89.0f, 89.0f);

        SDK::FRotator newRot(newPitch, newYaw, 0);
        localPC->SetControlRotation(newRot);
    }
}