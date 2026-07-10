#define NOMINMAX
#include "../framework.h"

namespace Framework::Widgets
{
    void Button(const char* label, const std::function<void()>& callback, float height)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImGuiID id = window->GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 size(avail.x, height);

        ImGui::PushID(id);
        ImGui::Selectable("##button", false, ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick, size);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();
        ImGui::PopID();

        if (clicked && callback)
            callback();

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
        ImVec2 textPos(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        ImVec4 textColor = Colors::Text;
        dl->AddText(textPos, ImGui::GetColorU32(textColor), label);
    }

    void Toggle(const char* label, bool& value, bool showPicker, ImColor* color, float height)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        ImGuiContext& g = *GImGui;
        const ImGuiID id = window->GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 size(avail.x, height);

        ImGui::PushID(id);

        ImGui::Selectable("##item", false, ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick, size);

        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();

        ImVec2 swatchPos(pos.x + size.x - 22.0f, pos.y + (size.y - 16.0f) * 0.5f);
        ImVec2 mouse = ImGui::GetIO().MousePos;

        bool overPicker = showPicker && mouse.x >= swatchPos.x && mouse.x <= swatchPos.x + 16.0f && mouse.y >= swatchPos.y && mouse.y <= swatchPos.y + 16.0f;

        if (clicked && !overPicker)
            value = !value;

        float dt = ImGui::GetIO().DeltaTime;
        AnimData& anim = g_AnimCache[id];

        ImVec4 targetBgColor;
        if (active)
            targetBgColor = ImVec4(0.14f, 0.14f, 0.14f, 0.9f);
        else if (hovered)
            targetBgColor = ImVec4(0.01f, 0.01f, 0.01f, 0.25f);
        else if (value)
            targetBgColor = ImVec4(0.10f, 0.10f, 0.11f, 0.40f);
        else
            targetBgColor = ImVec4(0.10f, 0.10f, 0.11f, 0.00f);

        const float bgSpeed = 12.0f;
        const float barSpeed = 12.0f;

        anim.currentBgColor.x += (targetBgColor.x - anim.currentBgColor.x) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.y += (targetBgColor.y - anim.currentBgColor.y) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.z += (targetBgColor.z - anim.currentBgColor.z) * ImMin(1.0f, dt * bgSpeed);
        anim.currentBgColor.w += (targetBgColor.w - anim.currentBgColor.w) * ImMin(1.0f, dt * bgSpeed);

        float targetBarWidth = value ? 3.0f : 0.0f;
        anim.currentBarWidth += (targetBarWidth - anim.currentBarWidth) * ImMin(1.0f, dt * barSpeed);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (anim.currentBgColor.w > 0.001f)
            dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(anim.currentBgColor));

        if (anim.currentBarWidth > 0.001f)
            dl->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + anim.currentBarWidth, pos.y + size.y), ImGui::GetColorU32(Colors::Accent));

        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        ImVec4 textColor = value ? Colors::Text : Colors::TextSecondary;
        dl->AddText(textPos, ImGui::GetColorU32(textColor), label);

        if (showPicker && color)
        {
            ImVec4 col = ImGui::ColorConvertU32ToFloat4(*color);

            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImGui::SetCursorScreenPos(swatchPos);

            if (ImGui::InvisibleButton("##colorpicker", ImVec2(16.0f, 16.0f)))
                ImGui::OpenPopup("##pickerpopup");

            dl->AddRectFilled(swatchPos, ImVec2(swatchPos.x + 16.0f, swatchPos.y + 16.0f), ImGui::GetColorU32(col), 2.0f);
            dl->AddRect(swatchPos, ImVec2(swatchPos.x + 16.0f, swatchPos.y + 16.0f), IM_COL32(0, 0, 0, 100), 2.0f);

            if (ImGui::BeginPopup("##pickerpopup"))
            {
                ImGui::ColorPicker4("##picker", &col.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
                *color = ImGui::ColorConvertFloat4ToU32(col);
                ImGui::EndPopup();
            }

            ImGui::SetCursorPos(cursorPos);
        }

        ImGui::PopID();
    }

    void MenuExpandableItem(const char* label, bool& value, bool& expanded, const std::function<void()>& content, float height, float expandedHeight)
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

        AnimData& anim = g_AnimCache[id];

        bool arrowClicked = false;
        if (clicked)
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            float arrowLeft = pos.x + size.x - 25.0f;
            if (mousePos.x >= arrowLeft)
                arrowClicked = true;
            else
                value = !value;
        }

        if (arrowClicked)
        {
            expanded = !expanded;
            if (expanded)
                anim.scrollActive = true;
            else
                anim.scrollActive = false;
        }

        float dt = ImGui::GetIO().DeltaTime;
        float speed = 16.0f;
        float t = 1.0f - expf(-speed * dt);

        float targetAlpha = 0.0f;
        if (active) targetAlpha = 0.90f;
        else if (hovered) targetAlpha = 0.55f;
        else if (value) targetAlpha = 0.30f;

        anim.currentBgColor.w = ImLerp(anim.currentBgColor.w, targetAlpha, t);

        float targetBarWidth = value ? 3.0f : 0.0f;
        anim.currentBarWidth = ImLerp(anim.currentBarWidth, targetBarWidth, t);

        float targetExpand = expanded ? 1.0f : 0.0f;
        anim.expandProgress = ImLerp(anim.expandProgress, targetExpand, t);

        if (anim.expandProgress < 0.001f) anim.expandProgress = 0.0f;
        if (anim.expandProgress > 0.999f) anim.expandProgress = 1.0f;

        ImVec4 targetArrowColor = expanded ? Colors::Text : Colors::TextSecondary;
        anim.currentArrowColor.x = ImLerp(anim.currentArrowColor.x, targetArrowColor.x, t);
        anim.currentArrowColor.y = ImLerp(anim.currentArrowColor.y, targetArrowColor.y, t);
        anim.currentArrowColor.z = ImLerp(anim.currentArrowColor.z, targetArrowColor.z, t);
        anim.currentArrowColor.w = ImLerp(anim.currentArrowColor.w, targetArrowColor.w, t);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImVec4 bg = Colors::Surface;
        bg.w = anim.currentBgColor.w;
        dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::GetColorU32(bg));

        if (anim.currentBarWidth > 0.001f)
            dl->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + anim.currentBarWidth, pos.y + size.y), ImGui::GetColorU32(Colors::Accent));

        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        dl->AddText(textPos, ImGui::GetColorU32(value ? Colors::Text : Colors::TextSecondary), label);

        const char* arrowIcon = expanded ? ICON_FA_ANGLE_DOWN : ICON_FA_ANGLE_RIGHT;
        ImVec2 arrowSize = ImGui::CalcTextSize(arrowIcon);
        ImVec2 arrowPos(pos.x + size.x - 22.0f, pos.y + (size.y - arrowSize.y) * 0.5f);
        dl->AddText(arrowPos, ImGui::GetColorU32(anim.currentArrowColor), arrowIcon);

        float childHeight = expandedHeight * anim.expandProgress;

        if (anim.expandProgress > 0.001f && childHeight > 0.001f)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 4.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));
            ImGui::BeginChild(ImGui::GetID("##expand"), ImVec2(size.x, childHeight), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            content();
            ImGui::EndChild();
            ImGui::PopStyleVar(2);

            ImVec2 childPos = ImGui::GetItemRectMin();
            ImVec2 childSize = ImGui::GetItemRectSize();
            ImVec4 barColor = Colors::Accent;
            barColor.w = 0.5f;
            dl->AddRectFilled(ImVec2(childPos.x, childPos.y), ImVec2(childPos.x + 3.0f, childPos.y + childSize.y), ImGui::GetColorU32(barColor));

            if (expanded && anim.scrollActive)
            {
                float windowContentHeight = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
                float windowPosY = ImGui::GetWindowPos().y;
                ImVec2 childMax = ImGui::GetItemRectMax();
                float targetScroll = (childMax.y - windowPosY) - windowContentHeight + 10.0f;
                float maxScroll = ImGui::GetScrollMaxY();
                if (targetScroll > maxScroll) targetScroll = maxScroll;
                if (targetScroll < 0.0f) targetScroll = 0.0f;

                float currentScroll = ImGui::GetScrollY();
                bool userScrolling = ImGui::IsWindowHovered() && (ImGui::GetIO().MouseWheel != 0.0f || ImGui::IsMouseDragging(ImGuiMouseButton_Left));
                if (!userScrolling)
                {
                    float scrollSpeed = 18.0f;
                    float scrollLerp = 1.0f - expf(-scrollSpeed * dt);
                    float newScroll = ImLerp(currentScroll, targetScroll, scrollLerp);
                    ImGui::SetScrollY(newScroll);
                    if (anim.expandProgress >= 1.0f && fabs(newScroll - targetScroll) < 1.0f)
                    {
                        ImGui::SetScrollY(targetScroll);
                        anim.scrollActive = false;
                    }
                }
                else
                {
                    anim.scrollActive = false;
                }
            }
        }
        else
        {
            if (!expanded)
                anim.scrollActive = false;
        }
    }

    void Slider(const char* label, float* value, float min, float max, float step)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f); // padding from previous item in menu

        static std::map<ImGuiID, float> animThumb;
        static std::map<ImGuiID, float> targetThumb;

        ImGuiID id = window->GetID(label);

        float& thumbPos = animThumb[id];
        float& target = targetThumb[id];

        if (thumbPos == 0.0f) thumbPos = (*value - min) / (max - min);
        if (target == 0.0f) target = thumbPos;

        float padLabel = 10.0f; // padding on edge of window to text
        float margin = 12.0f;
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;

        ImGui::PushID(id);
        ImGui::SetCursorScreenPos(ImVec2(pos.x + padLabel, pos.y));
        ImGui::TextColored(ImVec4(Colors::Text), label);
        ImGui::Dummy(ImVec2(0.0f, 7.0f));

        float barWidth = width - margin * 2.0f;
        float barHeight = 4.0f;
        ImVec2 barStart = ImGui::GetCursorScreenPos();
        float barY = barStart.y + (20.0f - barHeight) * 0.5f;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 barMin = ImVec2(barStart.x + margin, barY);
        ImVec2 barMax = ImVec2(barStart.x + margin + barWidth, barY + barHeight);

        dl->AddRectFilled(barMin, barMax, ImGui::GetColorU32(ImVec4(Colors::Surface)), barHeight * 0.5f);

        float t = (*value - min) / (max - min);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        float speed = 12.0f;
        float dt = ImGui::GetIO().DeltaTime;

        if (ImGui::IsMouseDown(0))
        {
            ImGui::SetCursorScreenPos(ImVec2(barStart.x, barStart.y));
            ImGui::InvisibleButton("##slider", ImVec2(barWidth + margin * 2.0f, 20.0f));
            if (ImGui::IsItemActive())
            {
                float mouseX = ImGui::GetMousePos().x - barMin.x;
                float newT = mouseX / barWidth;
                if (newT < 0.0f) newT = 0.0f;
                if (newT > 1.0f) newT = 1.0f;
                target = newT;
            }
        }

        thumbPos += (target - thumbPos) * std::min(1.0f, dt * speed);

        float fillWidth = thumbPos * barWidth;

        ImVec2 fillMin = barMin;
        ImVec2 fillMax = ImVec2(barMin.x + fillWidth, barMax.y);
        dl->AddRectFilled(fillMin, fillMax, ImGui::GetColorU32(ImVec4(Colors::Accent)), barHeight * 0.5f);

        float thumbX = barMin.x + fillWidth;
        float thumbY = barMin.y + barHeight * 0.5f;
        float thumbRadius = 8.0f;

        dl->AddCircleFilled(ImVec2(thumbX, thumbY), thumbRadius, ImGui::GetColorU32(ImVec4(Colors::Accent)));

        *value = min + (max - min) * thumbPos;

        char txt[32];
        sprintf_s(txt, "%.2f", *value);
        ImVec2 txtSize = ImGui::CalcTextSize(txt);
        ImVec2 txtPos(barMax.x - txtSize.x - 2.0f, barMin.y + (barHeight - txtSize.y) * 0.5f);
        dl->AddText(txtPos, ImGui::GetColorU32(ImVec4(Colors::Text)), txt);

        ImGui::SetCursorScreenPos(ImVec2(barStart.x, barStart.y));
        ImGui::InvisibleButton("##slider_bg", ImVec2(barWidth + margin * 2.0f, 20.0f));

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);
        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        ImGui::PopID();
    }

    void BoolSelect(const char* label, bool& value, const char* trueLabel, const char* falseLabel)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        const ImGuiID id = window->GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 size(avail.x, 30.0f);

        ImGui::PushID(id);
        ImGui::Selectable("##boolselect", false, ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick, size);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();
        ImGui::PopID();

        if (clicked)
            value = !value;

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

        const char* currentText = value ? trueLabel : falseLabel;
        char fullText[256];
        snprintf(fullText, sizeof(fullText), "%s: %s", label, currentText);

        ImVec2 textSize = ImGui::CalcTextSize(fullText);
        ImVec2 textPos(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        dl->AddText(textPos, ImGui::GetColorU32(ImVec4(Colors::Text)), fullText);
    }

    void Keybind(const char* label, ImGuiKey& key)
    {
        static std::map<ImGuiID, bool> waitingForInput;
        static std::map<ImGuiID, bool> ignoreNextInput;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) return;

        const ImGuiID id = window->GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 size(avail.x, 30.0f);

        bool& isWaiting = waitingForInput[id];
        bool& ignoreNext = ignoreNextInput[id];

        ImGui::PushID(id);
        ImGui::Selectable("##keybind", false, ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick, size);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();
        bool clicked = ImGui::IsItemClicked();
        ImGui::PopID();

        if (clicked)
        {
            if (isWaiting)
            {
                isWaiting = false;
                ignoreNext = false;
            }
            else
            {
                isWaiting = true;
                ignoreNext = true;
            }
        }

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

        const char* keyName = ImGui::GetKeyName(key);
        if (!keyName || keyName[0] == '\0') keyName = "None";
        const char* displayText = isWaiting ? "Press any key..." : keyName;

        char fullText[256];
        snprintf(fullText, sizeof(fullText), "%s: %s", label, displayText);

        ImVec2 textSize = ImGui::CalcTextSize(fullText);
        ImVec2 textPos(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
        dl->AddText(textPos, ImGui::GetColorU32(ImVec4(Colors::Text)), fullText);

        if (isWaiting)
        {
            if (ignoreNext)
            {
                ignoreNext = false;
            }
            else
            {
                for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; ++i)
                {
                    if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(i)))
                    {
                        if (i == ImGuiKey_Escape)
                        {
                            isWaiting = false;
                            ignoreNext = false;
                            break;
                        }
                        key = static_cast<ImGuiKey>(i);
                        isWaiting = false;
                        ignoreNext = false;
                        break;
                    }
                }
                if (!isWaiting) return;
                for (int i = ImGuiKey_MouseLeft; i <= ImGuiKey_MouseX2; ++i)
                {
                    if (ImGui::IsMouseClicked(i - ImGuiKey_MouseLeft))
                    {
                        key = static_cast<ImGuiKey>(i);
                        isWaiting = false;
                        ignoreNext = false;
                        break;
                    }
                }
            }
        }
    }
}