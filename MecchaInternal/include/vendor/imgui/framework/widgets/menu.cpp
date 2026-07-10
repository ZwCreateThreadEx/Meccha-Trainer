#include "../framework.h"

namespace Framework::Widgets
{
    std::map<ImGuiID, AnimData> g_AnimCache;
    ImVec2 g_MainMenuPos = ImVec2(0, 0);
    ImVec2 g_MainMenuSize = ImVec2(0, 0);

    void MenuWidget(const char* icon, const char* title, const std::function<void()>& content, float width)
    {
        const float titleBarHeight = 28.0f;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0), ImVec2(width, FLT_MAX));
        ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

        ImGui::SetWindowSize(ImVec2(width, 0), ImGuiCond_Always);

        g_MainMenuPos = ImGui::GetWindowPos();
        g_MainMenuSize = ImGui::GetWindowSize();

        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSize = ImGui::GetWindowSize();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + titleBarHeight), ImGui::GetColorU32(Colors::Accent));

        if (icon)
        {
            ImVec2 iconSize = ImGui::CalcTextSize(icon);
            ImVec2 iconPos(winPos.x + 10.0f, winPos.y + (titleBarHeight - iconSize.y) * 0.5f);
            dl->AddText(iconPos, ImGui::GetColorU32(Colors::White), icon);
        }

        ImVec2 titleSize = ImGui::CalcTextSize(title);
        ImVec2 titlePos(winPos.x + (winSize.x - titleSize.x) * 0.5f, winPos.y + (titleBarHeight - titleSize.y) * 0.5f);
        dl->AddText(titlePos, ImGui::GetColorU32(Colors::White), title);

        ImGui::SetCursorPosY(titleBarHeight);

        content();

        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    void MenuAttachableWidget(const char* icon, const char* title, const std::function<void()>& content, float width, float offsetX, float offsetY)
    {
        const float titleBarHeight = 28.0f;

        ImVec2 pos = ImVec2(g_MainMenuPos.x + g_MainMenuSize.x + offsetX, g_MainMenuPos.y + offsetY);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0), ImVec2(width, FLT_MAX));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));

        ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSize = ImGui::GetWindowSize();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + titleBarHeight), ImGui::GetColorU32(Colors::Accent));

        if (icon)
        {
            ImVec2 iconSize = ImGui::CalcTextSize(icon);
            ImVec2 iconPos(winPos.x + 10.0f, winPos.y + (titleBarHeight - iconSize.y) * 0.5f);
            dl->AddText(iconPos, ImGui::GetColorU32(Colors::White), icon);
        }

        ImVec2 titleSize = ImGui::CalcTextSize(title);
        ImVec2 titlePos(winPos.x + (winSize.x - titleSize.x) * 0.5f, winPos.y + (titleBarHeight - titleSize.y) * 0.5f);
        dl->AddText(titlePos, ImGui::GetColorU32(Colors::White), title);

        ImGui::SetCursorPosY(titleBarHeight);
        content();

        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    void MenuTab(const char* label, int targetPage, int& currentPage, float height)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiID id = window->GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 size(avail.x, height);

        ImGui::PushID(id);
        ImGui::Selectable("##tab", false, ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick, size);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();
        ImGui::PopID();

        if (clicked)
            currentPage = targetPage;

        float dt = ImGui::GetIO().DeltaTime;
        AnimData& anim = g_AnimCache[id];

        ImVec4 targetBgColor;
        if (active)
            targetBgColor = ImVec4(0.14f, 0.14f, 0.14f, 0.9f);
        else if (hovered)
            targetBgColor = ImVec4(0.01f, 0.01f, 0.01f, 0.25f);
        else
            targetBgColor = ImVec4(0.10f, 0.10f, 0.11f, 0.00f);

        const float bgSpeed = 12.0f;

        anim.currentBgColor.x += (targetBgColor.x - anim.currentBgColor.x) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.y += (targetBgColor.y - anim.currentBgColor.y) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.z += (targetBgColor.z - anim.currentBgColor.z) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.w += (targetBgColor.w - anim.currentBgColor.w) * ImMin(1.0f, dt * bgSpeed);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (anim.currentBgColor.w > 0.001f)
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(anim.currentBgColor));

        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos(
            pos.x + (size.x - textSize.x) * 0.5f,
            pos.y + (size.y - textSize.y) * 0.5f
        );

        dl->AddText(textPos, ImGui::GetColorU32(Colors::Text), label);

        const char* arrowIcon = ICON_FA_ANGLE_RIGHT;
        ImVec2 arrowSize = ImGui::CalcTextSize(arrowIcon);
        ImVec2 arrowPos(
            pos.x + size.x - 22.0f,
            pos.y + (size.y - arrowSize.y) * 0.5f
        );

        dl->AddText(arrowPos, ImGui::GetColorU32(Colors::TextSecondary), arrowIcon);
    }

    void MenuBack(const char* label, int targetPage, int& currentPage, float height)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiID id = window->GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 size(avail.x, height);

        ImGui::PushID(id);
        ImGui::Selectable("##back", false, ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick, size);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();
        ImGui::PopID();

        if (clicked)
            currentPage = targetPage;

        float dt = ImGui::GetIO().DeltaTime;
        AnimData& anim = g_AnimCache[id];

        ImVec4 targetBgColor;
        if (active)
            targetBgColor = ImVec4(0.14f, 0.14f, 0.14f, 0.9f);
        else if (hovered)
            targetBgColor = ImVec4(0.01f, 0.01f, 0.01f, 0.25f);
        else
            targetBgColor = ImVec4(0.10f, 0.10f, 0.11f, 0.00f);

        const float bgSpeed = 12.0f;
        anim.currentBgColor.x += (targetBgColor.x - anim.currentBgColor.x) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.y += (targetBgColor.y - anim.currentBgColor.y) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.z += (targetBgColor.z - anim.currentBgColor.z) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.w += (targetBgColor.w - anim.currentBgColor.w) * ImMin(1.0f, dt * bgSpeed);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (anim.currentBgColor.w > 0.001f)
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(anim.currentBgColor));

        const char* arrowIcon = ICON_FA_ANGLE_LEFT;
        ImVec2 arrowSize = ImGui::CalcTextSize(arrowIcon);
        ImVec2 arrowPos(pos.x + 8.0f, pos.y + (size.y - arrowSize.y) * 0.5f);
        dl->AddText(arrowPos, ImGui::GetColorU32(Colors::TextSecondary), arrowIcon);

        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        dl->AddText(textPos, ImGui::GetColorU32(Colors::Text), label);
    }
}