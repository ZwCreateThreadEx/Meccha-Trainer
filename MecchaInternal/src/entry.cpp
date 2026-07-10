// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <include.h>
#include <string>
#include <fstream>
#include <cstdint>

#include <vendor/SDK.hpp>
#include <vendor/SDK/CoreUObject_classes.hpp>
#include <vendor/SDK/Engine_classes.hpp>

#include "classes/classes.h"
#include "globals.h"

std::string title = std::string("MecchaInternal - build: ") + __DATE__ + " " + __TIME__;

DWORD WINAPI Entry(LPVOID lpReserved) {
    FreeConsole();
    AllocConsole();
    FILE* fp;

    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    SetConsoleTitleA(title.c_str());

    Logger::Log(Level::info, "initializing...");

    gui::hook::attach();

    /*{
        uintptr_t GObjectsAddress = SDK::InSDKUtils::GetImageBase() + SDK::Offsets::GObjects;
        SDK::TUObjectArray* GObjects = reinterpret_cast<SDK::TUObjectArray*>(GObjectsAddress);
        if (GObjects)
        {
            std::ofstream file("materials_dump.txt");
            if (file.is_open())
            {
                int32_t num = GObjects->Num();
                for (int32_t i = 0; i < num; ++i)
                {
                    SDK::UObject* Obj = GObjects->GetByIndex(i);
                    if (!Obj) continue;
                    if (Obj->IsA(SDK::UMaterial::StaticClass()) || Obj->IsA(SDK::UMaterialInstance::StaticClass()))
                        file << Obj->GetFullName() << "\n";
                }
                file.close();
            }
        }
    }*/

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0, Entry, lpReserved, 0, 0);
    }
    return TRUE;
}

