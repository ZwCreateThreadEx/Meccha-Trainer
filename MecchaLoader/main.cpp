#include <windows.h>
#include <iostream>
#include <filesystem>
#include <tlhelp32.h>

DWORD FindProcessId(const wchar_t* processName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
    DWORD pid = 0;
    if (Process32FirstW(snapshot, &entry))
    {
        do
        {
            if (wcscmp(entry.szExeFile, processName) == 0)
            {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return pid;
}

int main()
{
    auto dllPath = std::filesystem::current_path() / "MecchaInternal.dll";

    if (!std::filesystem::exists(dllPath))
    {
        std::cout << "MecchaInternal.dll not found\n";
        Sleep(2000);
        return 0;
    }
    std::cout << "MecchaInternal.dll found\n";

    DWORD pid = FindProcessId(L"PenguinHotel-Win64-Shipping.exe");
    if (pid == 0)
    {
        std::cout << "Process not found\n";
        Sleep(2000);
        return 0;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess)
    {
        std::cout << "OpenProcess failed, error: " << GetLastError() << "\n";
        Sleep(2000);
        return 0;
    }

    std::string pathStr = dllPath.string();
    size_t pathSize = pathStr.size() + 1;

    void* remoteMem = VirtualAllocEx(hProcess, NULL, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem)
    {
        std::cout << "VirtualAllocEx failed, error: " << GetLastError() << "\n";
        CloseHandle(hProcess);
        Sleep(2000);
        return 0;
    }

    if (!WriteProcessMemory(hProcess, remoteMem, pathStr.c_str(), pathSize, NULL))
    {
        std::cout << "WriteProcessMemory failed, error: " << GetLastError() << "\n";
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        Sleep(2000);
        return 0;
    }

    FARPROC loadLib = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (!loadLib)
    {
        std::cout << "GetProcAddress failed, error: " << GetLastError() << "\n";
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        Sleep(2000);
        return 0;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLib, remoteMem, 0, NULL);
    if (!hThread)
    {
        std::cout << "CreateRemoteThread failed, error: " << GetLastError() << "\n";
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        Sleep(2000);
        return 0;
    }

    std::cout << "Injected successfully\n";
    CloseHandle(hThread);
    CloseHandle(hProcess);

    Sleep(2000);
    return 0;
}