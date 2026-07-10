#include <algorithm>
#include "framework.h"

namespace Framework
{
    namespace Draw
    {
        void Rect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, float rounding, float thickness)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();
            draw->AddRect(pMin, pMax, ImGui::GetColorU32(color), rounding, 0, thickness);
        }

        void Line(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2, const ImVec4& color, float thickness)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();
            draw->AddLine(p1, p2, ImGui::GetColorU32(color), thickness);
        }

        void OutlinedLine(ImDrawList* draw, const ImVec2& p1, const ImVec2& p2, const ImVec4& color, const ImVec4& outlineColor, float thickness, float outlineThickness)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();

            draw->AddLine(p1, p2, ImGui::GetColorU32(outlineColor), thickness + outlineThickness);

            draw->AddLine(p1, p2, ImGui::GetColorU32(color), thickness);
        }
        void Text(ImDrawList* draw, const ImVec2& pos, const char* text, const ImVec4& color, bool center)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();
            ImVec2 textSize = ImGui::CalcTextSize(text);
            ImVec2 finalPos = pos;
            if (center) finalPos.x -= textSize.x * 0.5f;
            draw->AddText(finalPos, ImGui::GetColorU32(color), text);
        }

        void TextOutlined(ImDrawList* draw, const ImVec2& pos, const char* text, const ImVec4& color, const ImVec4& outlineColor, float outlineSize, bool center)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();
            ImVec2 textSize = ImGui::CalcTextSize(text);
            ImVec2 finalPos = pos;
            if (center) finalPos.x -= textSize.x * 0.5f;

            for (float x = -outlineSize; x <= outlineSize; x += outlineSize)
                for (float y = -outlineSize; y <= outlineSize; y += outlineSize)
                    if (x != 0 || y != 0)
                        draw->AddText(ImVec2(finalPos.x + x, finalPos.y + y), ImGui::GetColorU32(outlineColor), text);

            draw->AddText(finalPos, ImGui::GetColorU32(color), text);
        }

        void OutlinedRect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, const ImVec4& outlineColor, float rounding, float thickness, float outlineThickness)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();
            draw->AddRect(pMin, pMax, ImGui::GetColorU32(outlineColor), rounding, 0, outlineThickness);
            draw->AddRect(pMin, pMax, ImGui::GetColorU32(color), rounding, 0, thickness);
        }

        void CorneredRect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, const ImVec4& outlineColor, float cornerSize, float thickness, float outlineThickness)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();

            float x = pMin.x, y = pMin.y, w = pMax.x - pMin.x, h = pMax.y - pMin.y;
            float halfMin = (w < h ? w : h) * 0.5f;
            float c = cornerSize > halfMin ? halfMin : cornerSize;

            ImU32 col = ImGui::GetColorU32(color);
            ImU32 outline = ImGui::GetColorU32(outlineColor);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x, y + c), ImVec2(x, y), outline, outlineThickness);
            draw->AddLine(ImVec2(x, y + c), ImVec2(x, y), col, thickness);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x, y), ImVec2(x + c, y), outline, outlineThickness);
            draw->AddLine(ImVec2(x, y), ImVec2(x + c, y), col, thickness);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x + w - c, y), ImVec2(x + w, y), outline, outlineThickness);
            draw->AddLine(ImVec2(x + w - c, y), ImVec2(x + w, y), col, thickness);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + c), outline, outlineThickness);
            draw->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + c), col, thickness);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x, y + h - c), ImVec2(x, y + h), outline, outlineThickness);
            draw->AddLine(ImVec2(x, y + h - c), ImVec2(x, y + h), col, thickness);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x, y + h), ImVec2(x + c, y + h), outline, outlineThickness);
            draw->AddLine(ImVec2(x, y + h), ImVec2(x + c, y + h), col, thickness);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x + w - c, y + h), ImVec2(x + w, y + h), outline, outlineThickness);
            draw->AddLine(ImVec2(x + w - c, y + h), ImVec2(x + w, y + h), col, thickness);

            if (outlineThickness > 0)
                draw->AddLine(ImVec2(x + w, y + h - c), ImVec2(x + w, y + h), outline, outlineThickness);
            draw->AddLine(ImVec2(x + w, y + h - c), ImVec2(x + w, y + h), col, thickness);
        }

        void FilledRect(ImDrawList* draw, const ImVec2& pMin, const ImVec2& pMax, const ImVec4& color, float rounding)
        {
            if (!draw) draw = ImGui::GetWindowDrawList();
            draw->AddRectFilled(pMin, pMax, ImGui::GetColorU32(color), rounding);
        }
    }
}