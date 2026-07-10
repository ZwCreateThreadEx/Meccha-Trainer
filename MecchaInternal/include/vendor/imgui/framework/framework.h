#pragma once

#include <map>
#include <functional>
#include <include.h>

#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_internal.h>

#include "../../../../src/classes/classes.h"

struct ID3D12Device;
struct ID3D12Resource;
struct ID3D12DescriptorHeap;

namespace Framework
{
    namespace Draw
    {
        void Rect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, float rounding = 0.0f, float thickness = 0.0f);

        void Line(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2, const ImVec4& color, float thickness = 1.0f);
        void OutlinedLine(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2, const ImVec4& color, const ImVec4& outlineColor, float thickness = 1.0f, float outlineThickness = 3.0f);
        
        void Text(ImDrawList* draw, const ImVec2& pos, const char* text, const ImVec4& color, bool center = false);
        void TextOutlined(ImDrawList* draw, const ImVec2& pos, const char* text, const ImVec4& color, const ImVec4& outlineColor = ImVec4(0, 0, 0, 1), float outlineSize = 1.0f, bool center = false);

        void OutlinedRect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, const ImVec4& outlineColor = ImVec4(0, 0, 0, 1), float rounding = 0.0f, float thickness = 1.5f, float outlineThickness = 3.0f);
        void CorneredRect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, const ImVec4& outlineColor = ImVec4(0, 0, 0, 1), float cornerSize = 8.0f, float thickness = 2.0f, float outlineThickness = 3.0f);
        void FilledRect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, float rounding = 0.0f);
    }

    namespace Helpers
    {

    }

    namespace Widgets
    {

        struct AnimData
        {
            ImVec4 currentBgColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            float currentBarWidth = 0.0f;
            float expandProgress = 0.0f;
            ImVec4 currentArrowColor = ImVec4(0.55f, 0.55f, 0.58f, 1.0f);
            bool scrollPending = false;
            float scrollTarget = 0.0f;
            bool scrollActive = false;
            bool autoScrolled = false;
        };

        extern std::map<ImGuiID, AnimData> g_AnimCache;
        extern ImVec2 g_MainMenuPos;
        extern ImVec2 g_MainMenuSize;

        void MenuWidget(const char* icon, const char* title, const std::function<void()>& content, float width = 210.0f);
        void MenuAttachableWidget(const char* icon, const char* title, const std::function<void()>& content, float width = 210.0f, float offsetX = 20.0f, float offsetY = 0.0f);
        void MenuExpandableItem(const char* label, bool& value, bool& expanded, const std::function<void()>& content, float height = 30.0f, float expandedHeight = 150.0f);
        void Button(const char* label, const std::function<void()>& callback, float height = 30.0f);
        void Toggle(const char* label, bool& value, bool showPicker = false, ImColor* color = nullptr, float height = 30.0f);

        // extended menu items
        void Slider(const char* label, float* value, float min, float max, float step = 0.01f);
        void BoolSelect(const char* label, bool& value, const char* trueLabel, const char* falseLabel);
        void Keybind(const char* label, ImGuiKey& key);

        void MenuTab(const char* label, int targetPage, int& currentPage, float height = 30.0f);
        void MenuBack(const char* label, int targetPage, int& currentPage, float height = 30.0f);

        void SearchBox(const char* label, char* buffer, size_t buffer_size, float height = 30.0f);
        void TabSeparator(const char* icon, const char* label);

        // player widgets
        void MenuPlayerEntry(const char* label, const Meccha::PlayerInfo& player, bool& selected, const std::function<void(bool)>& onToggle = nullptr, const char* prefix = nullptr, const ImVec4& prefixColor = ImVec4(0.55f, 0.55f, 0.58f, 1.0f), float height = 30.0f);
        void PlayerInfoWidget(const Meccha::PlayerInfo& player, float width = 200.0f, float offsetY = 0.0f);
    }

    namespace Fonts
    {
        inline ImFont* fontMontserrat = nullptr;
    }

    namespace Cache
    {
        struct RawImages
        {
            static std::map<const unsigned char*, ID3D12Resource*>& Get()
            {
                static std::map<const unsigned char*, ID3D12Resource*> cache;
                return cache;
            }
        };
    }

    namespace Colors
    {
        inline constexpr ImVec4 Accent = ImVec4(0.62f, 0.35f, 0.95f, 0.9f);

        inline constexpr ImVec4 Background = ImVec4(0.06f, 0.06f, 0.07f, 0.9f);
        inline constexpr ImVec4 Surface = ImVec4(0.10f, 0.10f, 0.11f, 1.0f);
        inline constexpr ImVec4 Border = ImVec4(0.16f, 0.16f, 0.18f, 1.0f);

        inline constexpr ImVec4 Text = ImVec4(0.92f, 0.92f, 0.93f, 1.0f);
        inline constexpr ImVec4 TextMini = ImVec4(0.35f, 0.35f, 0.48f, 1.0f);
        inline constexpr ImVec4 TextSecondary = ImVec4(0.55f, 0.55f, 0.58f, 1.0f);

        inline constexpr ImVec4 White = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        inline constexpr ImVec4 Black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        inline constexpr ImVec4 Success = ImVec4(0.30f, 0.75f, 0.40f, 1.0f);
        inline constexpr ImVec4 Warning = ImVec4(0.90f, 0.70f, 0.20f, 1.0f);
        inline constexpr ImVec4 Error = ImVec4(0.90f, 0.25f, 0.25f, 1.0f);
    }
}