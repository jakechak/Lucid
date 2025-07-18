#include "pch.h"
#include <Windows.h>        
#include <jni.h>          
#include <chrono>
#include <memory>

#include "MinHook.h"

#include "Client.h"
#include "Module.h"
#include "Reach.h"
#include "Autoclicker.h"
#include "Gui.h"

// Typedef for the original SwapBuffers function
typedef BOOL(WINAPI* SwapBuffers_t)(HDC hdc);
// Function pointer to hold the original SwapBuffers function
SwapBuffers_t SwapBuffersDefault = nullptr;

std::unique_ptr<Gui> gui;

//Minecraft only updates 20x per second. No need to run more often than that.
auto last_run = std::chrono::high_resolution_clock::now();

// Replacement function for SwapBuffers
BOOL WINAPI SwapBuffersOverride(HDC hdc)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_run);

    if (elapsed.count() < 50)
    {
        gui->Render();
        return SwapBuffersDefault(hdc);
    }

   if (Client::jvm->GetEnv((void**)&Client::env, JNI_VERSION_1_6) != JNI_OK)
        Client::jvm->AttachCurrentThread((void**)&Client::env, nullptr);

   //Might just throw this in a client function "Update"
    for (std::unique_ptr<Module>& m : Client::modules)
    {
        m->ListenForKeyPress();
        m->OnUpdate();
    }
    gui->Render();
    last_run = now;

    // Call the original SwapBuffers so the game continues rendering
    return SwapBuffersDefault(hdc);
}

//Main logic thread that sets up the hook
DWORD WINAPI MainThread(HMODULE hModule)
{
    // Initialize MinHook
    if (MH_Initialize() != MH_OK)
        return 1;

    // Get address of the SwapBuffers function from gdi32.dll
    void* SwapBuffersP = GetProcAddress(GetModuleHandleA("gdi32.dll"), "SwapBuffers");
    if (SwapBuffersP == nullptr)
        return 1;

    //Hook creation to replace SwapBuffers with our hkSwapBuffers function
    if (MH_CreateHook(SwapBuffersP, &SwapBuffersOverride, reinterpret_cast<void**>(&SwapBuffersDefault)) != MH_OK)
        return 1;

    // Enable the hook so it starts intercepting calls
    if (MH_EnableHook(SwapBuffersP) != MH_OK)
        return 1;

    //Fetches the JVM - assuming only 1
    if (JNI_GetCreatedJavaVMs(&Client::jvm, 1, nullptr) != JNI_OK || Client::jvm == nullptr)
        return 1;

    //Attaches our current thread to the JVM - we can now use JNI
    if (Client::jvm->AttachCurrentThread((void**)&Client::env, nullptr) != JNI_OK || Client::env == nullptr)
        return 1;

    Client::Init();

    gui = std::make_unique<Gui>();

    MessageBoxA(nullptr, "Lucid injected successfully.", "Lucid", MB_OK | MB_ICONINFORMATION);

    return 0;
}

// Entry point for the DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Disable calls to DllMain for thread create/destroy events (improves performance & stability)
        DisableThreadLibraryCalls(hModule);
        // Starting logic on a new thread to avoid doing work inside DllMain
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        //Disable hooks ??
        break;
    }
    return TRUE;
}

