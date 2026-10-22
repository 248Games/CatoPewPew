#include "cheat.h"
#include "renderer.h"
#include "d3d9hook.h"
#include <windows.h>

// DllMain runs when Extreme Injector (or LoadLibrary) loads the DLL.
BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hMod);
        // Defer hooking to a thread so we don't hold the loader lock.
        CreateThread(nullptr, 0, [](LPVOID p) -> DWORD {
            Sleep(1500); // wait for game to create its D3D device
            while (true) {
                HWND hwnd = FindWindowA("AppWindowClass", nullptr); // catopewpew class (adjust)
                if (!hwnd) { Sleep(500); continue; }
                HMODULE d3d = GetModuleHandleA("d3d9.dll");
                if (d3d) break;
                Sleep(500);
            }
            return 0;
        }, nullptr, 0, nullptr);
    }
    return TRUE;
}
