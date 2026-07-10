#pragma once
#include <vendor/imgui/imgui.h>

namespace Globals
{
	namespace Settings
	{
		inline bool g_menu = true;
		inline bool g_watermark = true;
		inline bool g_debugMenu = true;
		inline bool g_menuBackground = true;

		inline ImGuiKey menuKeybind = ImGuiKey_Insert;
	}

	namespace Features
	{
		namespace Aimbot
		{
			inline bool enabled = false;
			inline bool hunterOnly = true;

			inline bool fovCircle = true;

			inline bool aimMode = false;  // false == toggle / true == hold
			inline bool aimActive = false; // internal aimbot toggle
			inline ImGuiKey aimKey = ImGuiKey_V;

			inline float smoothness = 3.5f;
			inline float fov = 30.0f;

			inline int bone = 0;
		}

		namespace ESP
		{
			inline bool enabled = true;

			inline bool box = true;
			inline bool filled = false;
			inline bool cornered = false;
			inline bool bounding = true;

			inline bool skeleton = false;

			inline bool chams = false;

			inline bool names = true;
			inline bool teams = true;
			inline bool outline = true;
			inline bool distance = true;

			inline ImColor boxColor = ImColor(255, 255, 255, 255);
			inline ImColor fillColor = ImColor(255, 255, 255, 50);
			inline ImColor outlineColor = ImColor(0, 0, 0, 200);
			inline ImColor nameColor = ImColor(255, 255, 255, 255);
			inline ImColor hunterColor = ImColor(255, 80, 80, 255);
			inline ImColor hiderColor = ImColor(200, 200, 200, 255);
		}

		namespace Exploits
		{
			inline bool rapid_fire = false;
			inline bool instant_win = false;
		}
	}
}
