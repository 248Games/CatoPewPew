#include "d3d9hook.h"
#include "cheat.h"

HRESULT(__stdcall* D3D9Hook::g_origPresent)(LPDIRECT3DDEVICE9, LPCRECT) = nullptr;
HRESULT(__stdcall* D3D9Hook::g_origReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*) = nullptr;

static void HookTrampoline(void** target, void* detour, void** origOut) {
    BYTE* t = (BYTE*)target;
    DWORD oldProt; VirtualProtect(t, 5, PAGE_EXECUTE_READWRITE, &oldProt);
    t[0] = 0xE9;
    *(DWORD*)(t + 1) = (DWORD)((BYTE*)detour - (t + 5));
    *origOut = (void**)(t + 5);
    VirtualProtect(t, 5, oldProt, &oldProt);
}

HRESULT __stdcall HookedPresent(LPDIRECT3DDEVICE9 dev, LPCRECT rect) {
    Cheat::Frame(dev);
    return D3D9Hook::g_origPresent(dev, rect);
}
HRESULT __stdcall HookedReset(LPDIRECT3DDEVICE9 dev, D3DPRESENT_PARAMETERS* p) {
    return D3D9Hook::g_origReset(dev, p);
}

void D3D9Hook::Install(IDirect3DDevice9* dev) {
    void** vt = *(void***)dev;
    HookTrampoline(&vt[16], (void*)HookedPresent, (void**)&g_origPresent);
    HookTrampoline(&vt[17], (void*)HookedReset, (void**)&g_origReset);
}
