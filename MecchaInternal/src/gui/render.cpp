// render.cpp
#include <Windows.h>
#include <iostream>

#include <include.h>
#include "../globals.h"
#include "../classes/classes.h"
#include "../cheats/cheats.h"

using namespace Framework;
using namespace Meccha;
using namespace Globals;

void gui::hook::init()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    io.IniFilename = nullptr;
    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    //io.WantCaptureMouse = Settings::g_menu;

    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;

    static const ImWchar ranges[] = { 0x0020, 0x00FF, 0 };
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

    float iconFontSize = 16.5f;

    ImFontConfig icons_cfg;
    icons_cfg.MergeMode = true;
    icons_cfg.PixelSnapH = true;
    icons_cfg.GlyphMinAdvanceX = iconFontSize;

    style.WindowRounding = 0.0f;
    style.FrameRounding = 2.0f;
    style.ChildRounding = 5.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 3.0f;

    style.Colors[ImGuiCol_Text] = Framework::Colors::Text;
    style.Colors[ImGuiCol_TextDisabled] = Framework::Colors::TextSecondary;
    style.Colors[ImGuiCol_WindowBg] = Framework::Colors::Background;
    style.Colors[ImGuiCol_ChildBg] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_PopupBg] = Framework::Colors::Background;
    style.Colors[ImGuiCol_Border] = Framework::Colors::Border;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.22f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_TitleBgActive] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_TitleBgCollapsed] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_MenuBarBg] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_ScrollbarBg] = Framework::Colors::Background;
    style.Colors[ImGuiCol_ScrollbarGrab] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.22f, 0.22f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.28f, 0.28f, 0.30f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_SliderGrab] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_SliderGrabActive] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_Button] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.22f, 0.22f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_Header] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.22f, 0.22f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_Separator] = Framework::Colors::Border;
    style.Colors[ImGuiCol_SeparatorHovered] = Framework::Colors::Border;
    style.Colors[ImGuiCol_SeparatorActive] = Framework::Colors::Border;
    style.Colors[ImGuiCol_ResizeGrip] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.22f, 0.22f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_Tab] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_TabUnfocused] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_TabUnfocusedActive] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_PlotLines] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_PlotLinesHovered] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_PlotHistogram] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_PlotHistogramHovered] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_TableHeaderBg] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_TableBorderStrong] = Framework::Colors::Border;
    style.Colors[ImGuiCol_TableBorderLight] = Framework::Colors::Border;
    style.Colors[ImGuiCol_TableRowBg] = Framework::Colors::Background;
    style.Colors[ImGuiCol_TableRowBgAlt] = Framework::Colors::Surface;
    style.Colors[ImGuiCol_TextSelectedBg] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_DragDropTarget] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_NavHighlight] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_NavWindowingHighlight] = Framework::Colors::Accent;
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

    io.Fonts->AddFontFromMemoryTTF(MontserratData, MontserratLen, 16.0f, &cfg, ranges);
    io.Fonts->AddFontFromMemoryTTF(fontawesomeWebfontTtf, fontawesomeWebfontTtfLen, iconFontSize, &icons_cfg, icons_ranges);
}

void gui::hook::render()
{
    static int currentPage = 0;
    static std::string selectedPlayerName;

    Meccha::PlayerManager::Invalidate(); // clear cache every frame for updates

    // render esp in main render loop if enabled
    if (Features::ESP::enabled) {
        Cheats::ESP::Draw();
    }

    if (Features::Aimbot::enabled) {
        Cheats::Aimbot::Run();
    }

    if (Settings::g_menu)
    {
        auto& io = ImGui::GetIO();
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        bg->AddRectFilled(ImVec2(0.0f, 0.0f), io.DisplaySize, IM_COL32(0, 0, 0, 100));

        constexpr float menuWidth = 210.0f;
        constexpr float menuSpacing = 20.0f;
        constexpr ImVec2 startPos(40.0f, 40.0f);

        ImGui::SetNextWindowPos(startPos, ImGuiCond_FirstUseEver);

        Widgets::MenuWidget(ICON_FA_EYE, "Visuals", [&]() {
            Widgets::MenuBack("Back", 0, currentPage);
            Widgets::Toggle("Enabled", Features::ESP::enabled);
            Widgets::Toggle("Boxes", Features::ESP::box, true, &Features::ESP::boxColor);
            Widgets::Toggle("Filled", Features::ESP::filled, true, &Features::ESP::fillColor);
            Widgets::Toggle("Outlines", Features::ESP::outline);
            Widgets::Toggle("Cornered", Features::ESP::cornered);
            Widgets::Toggle("Team colors", Features::ESP::teams, true, &Features::ESP::hunterColor);
            Widgets::Toggle("Skeletons", Features::ESP::skeleton, true, &Features::ESP::boxColor);
            //Widgets::Toggle("Chams", Features::ESP::chams);
            Widgets::Toggle("Distance", Features::ESP::distance);
            Widgets::Toggle("Player names", Features::ESP::names);
            });

        ImGui::SetNextWindowPos(ImVec2(startPos.x + (menuWidth + menuSpacing), startPos.y), ImGuiCond_FirstUseEver);

        Widgets::MenuWidget(ICON_FA_CROSSHAIRS, "Aimbot", [&]() {
            Widgets::MenuBack("Back", 0, currentPage);
            Widgets::Toggle("Enabled", Features::Aimbot::enabled);
            Widgets::Toggle("Hunter only", Features::Aimbot::hunterOnly);
            Widgets::Toggle("FOV Circle", Features::Aimbot::fovCircle);
            Widgets::Slider("FOV", &Features::Aimbot::fov, 5.0f, 50.0f);
            Widgets::Slider("Smoothness", &Features::Aimbot::smoothness, 0.0f, 7.5f);
            Widgets::BoolSelect("Aim Mode", Features::Aimbot::aimMode, "Hold", "Toggle");
            Widgets::Keybind("Aim Key", Features::Aimbot::aimKey);
            });

        ImGui::SetNextWindowPos(ImVec2(startPos.x + (menuWidth + menuSpacing) * 2.0f, startPos.y), ImGuiCond_FirstUseEver);

        Widgets::MenuWidget(ICON_FA_COGS, "Settings", [&]() {
            Widgets::Keybind("Menu keybind", Settings::menuKeybind);
            });
    }
}