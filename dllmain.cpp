// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "memory.h"


DWORD WINAPI HackThread(HMODULE hModule)
{
    // Create a console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Console created successfully.\n";

    // Get module base address
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    bool bArsenal = false;
    bool bRecoil = false;

    // Create a hack loop
    while (true)
    {
        // Get key input
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }

        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bArsenal = !bArsenal;
        }

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                memory::NOP((BYTE*)(moduleBase + 0x63786), 10);
            }

            else
            {
                memory::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
            }
        }

        uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10f4f4);

        if (localPlayerPtr)
        {
            if (bArsenal)
            {
                *(int*)(*localPlayerPtr + 0xf8) = 999;
                *(int*)(*localPlayerPtr + 0xfc) = 99;
                *(int*)(*localPlayerPtr + 0x158) = 99;
                *(int*)memory::FindDMAAddress(moduleBase + 0x10f4f4, { 0x374, 0x14, 0x0 }) = 99;
            }
        }
        Sleep(5);
    }

    // Clean up & eject
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

