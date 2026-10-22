#pragma once
#include "game.h"
#include "d3d9hook.h"      // your D3DXVECTOR2/3 + math already live here
#include "d3dx9font_shim.h"  // create this next

struct EntityScreen {
    G::Entity* ent;
    D3DXVECTOR2 screen;   // 2D projected position (head)
    float dist;
    bool onScreen;
};

class Renderer {
public:
    static void Init(IDirect3DDevice9* dev);
    static void EndScene(LPDIRECT3DDEVICE9 dev);   // NOTE: LPDIRECT3DDEVICE9, not LPDDIRECT3DDEVICE9
    static bool WorldToScreen(const G::Cam& cam, const float* world, D3DXVECTOR2* out);
    static ID3DXFont* GetFont();
private:
    static ID3DXFont* g_font;
    static IDirect3DDevice9* g_dev;
};
