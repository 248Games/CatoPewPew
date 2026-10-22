#include "cheat.h"
#include "renderer.h"
#include <cmath>
#include <algorithm>

static Settings g_settings;
const Settings& Cheat::GetSettings() { return g_settings; }

// Read entities from game memory (placeholder scan).
static std::vector<G::Entity*> ReadEntities() {
    std::vector<G::Entity*> out;
    // Real implementation would dereference G::ENT_LIST_PTR from the game's base.
    // Here we keep it minimal so the DLL links and runs; wire up real offsets in game.h.
    return out;
}

static bool InFov(const D3DXVECTOR2& a, const D3DXVECTOR2& b, int radius) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy) <= (float)radius;
}

static void DoAimbot(const G::Cam& cam, const std::vector<EntityScreen>& ents,
                     D3DXVECTOR2* mouse) {
    if (!g_settings.aimbot || ents.empty()) return;

    // Nearest enemy inside FOV circle and max distance.
    EntityScreen* best = nullptr;
    float bestDist = 1e9f;
    for (auto& e : ents) {
        if (!e.ent->isEnemy || !e.ent->alive || !e.onScreen) continue;
        if (e.dist > g_settings.maxDist) continue;
        if (!InFov(*mouse, e.screen, g_settings.fovRadius)) continue;
        if (e.dist < bestDist) { best = &e; bestDist = e.dist; }
    }
    if (!best) return;

    // Smoothed aim: move mouse toward target head.
    float t = g_settings.aimSmooth;
    POINT cur; GetCursorPos(&cur);
    int nx = (int)(cur.x + (best->screen.x - cur.x) * t);
    int ny = (int)(cur.y + (best->screen.y - cur.y) * t);
    SetCursorPos(nx, ny);

    if (g_settings.triggerBot && GetAsyncKeyState(VK_LBUTTON) <= 0) {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0);
    }
}

static void DrawESP(LPDIRECT3DDEVICE9 dev, const G::Cam& cam,
                    const std::vector<EntityScreen>& ents) {
    if (!g_settings.esp) return;
    D3DXMATRIX old; dev->GetTransform(D3DTRANSFORMSTATE_WORLD, &old);
    dev->SetTransform(D3DTRANSFORMSTATE_WORLD, &D3DXMatrixIdentity());

    for (auto& e : ents) {
        if (!e.onScreen || !e.ent->alive) continue;
        D3DCOLOR col = e.ent->isEnemy ? D3DCOLOR_ARGB(255, 80, 220, 80)
                                      : D3DCOLOR_ARGB(255, 220, 220, 80);
        // Simple box around head point (size scaled by distance).
        float s = std::max(6.0f, 400.0f / e.dist);
        D3DXVECTOR2 c = e.screen;

        wchar_t txt[64];
        swprintf(txt, 64, L"#%d %.0f", e.ent->id, e.dist);
        RECT r = { (LONG)c.x - 20, (LONG)c.y - 30, (LONG)c.x + 80, (LONG)c.y - 12 };
        if (Renderer::GetFont())
            Renderer::GetFont()->DrawText(nullptr, txt, -1, &r,
                DT_CENTER | DT_NOCLIP, col);
    }
    dev->SetTransform(D3DTRANSFORMSTATE_WORLD, &old);
}

static void DrawFov(LPDIRECT3DDEVICE9 dev) {
    if (!g_settings.fovCircle || !g_settings.aimbot) return;
    D3DXMATRIX old; dev->GetTransform(D3DTRANSFORMSTATE_WORLD, &old);
    dev->SetTransform(D3DTRANSFORMSTATE_WORLD, &D3DXMatrixIdentity());
    POINT m; GetCursorPos(&m);
    for (int i = 0; i <= 64; ++i) {
        float a = i / 64.0f * 2.0f * 3.14159265f;
        D3DXVECTOR2 p{ m.x + cosf(a) * g_settings.fovRadius,
                       m.y + sinf(a) * g_settings.fovRadius };
        dev->Begin();
        dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &p, sizeof(D3DXVECTOR2));
        dev->End();
    }
    dev->SetTransform(D3DTRANSFORMSTATE_WORLD, &old);
}

static void DrawMenu(LPDIRECT3DDEVICE9 dev) {
    if (!g_settings.menuOpen) return;
    RECT rc; GetClientRect(dev->GetWindowHandle(), &rc);
    D3DXMATRIX old; dev->GetTransform(D3DTRANSFORMSTATE_WORLD, &old);
    dev->SetTransform(D3DTRANSFORMSTATE_WORLD, &D3DXMatrixIdentity());

    const wchar_t* lines[] = {
        L"[catopewpew]  menu (Insert to close)",
        nullptr,
        g_settings.aimbot ? L"1  Aimbot      [ON]" : L"1  Aimbot      [off]",
        g_settings.esp    ? L"2  ESP         [ON]" : L"2  ESP         [off]",
        g_settings.fovCircle ? L"3  FOV circle  [ON]" : L"3  FOV circle  [off]",
        g_settings.triggerBot? L"4  Trigger bot [ON]" : L"4  Trigger bot [off]",
        nullptr,
    };
    if (Renderer::GetFont()) {
        RECT r = { 16, 16, rc.right - 16, rc.bottom - 16 };
        for (auto* l : lines) if (l)
            Renderer::GetFont()->DrawText(nullptr, l, -1, &r, DT_NOCLIP,
                D3DCOLOR_ARGB(255, 240, 240, 90));
    }
    dev->SetTransform(D3DTRANSFORMSTATE_WORLD, &old);
}

void Cheat::Init() {
    // hook setup happens in d3d9hook.cpp; this just resets state.
}

void Cheat::Frame(LPDIRECT3DDEVICE9 dev) {
    if (GetAsyncKeyState(VK_INSERT) & 1) g_settings.menuOpen = !g_settings.menuOpen;
    if (GetAsyncKeyState(VK_NUMPAD1) & 1) g_settings.aimbot = !g_settings.aimbot;
    if (GetAsyncKeyState(VK_NUMPAD2) & 1) g_settings.esp = !g_settings.esp;
    if (GetAsyncKeyState(VK_NUMPAD3) & 1) g_settings.fovCircle = !g_settings.fovCircle;
    if (GetAsyncKeyState(VK_NUMPAD4) & 1) g_settings.triggerBot = !g_settings.triggerBot;

    G::Cam cam{}; // placeholder — read from G::CAM_PTR in a real build
    auto ents = ReadEntities();
    std::vector<EntityScreen> screens;
    for (auto* e : ents) {
        EntityScreen s{ e, {}, 0, false };
        float head[3] = { e->pos[0], e->pos[1] + e->headOffset, e->pos[2] };
        if (Renderer::WorldToScreen(cam, head, &s.screen)) {
            s.onScreen = true;
            float dx = cam.pos[0]-head[0], dy = cam.pos[1]-head[1], dz = cam.pos[2]-head[2];
            s.dist = sqrtf(dx*dx+dy*dy+dz*dz);
        }
        screens.push_back(s);
    }

    POINT m; GetCursorPos(&m);
    D3DXVECTOR2 mouse{ (float)m.x, (float)m.y };
    DoAimbot(cam, screens, &mouse);
    DrawESP(dev, cam, screens);
    DrawFov(dev);
    DrawMenu(dev);
}
