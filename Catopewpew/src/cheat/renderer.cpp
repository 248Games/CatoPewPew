#include "renderer.h"
#include <d3dx9.h>   // for D3DXCreateFont only
#include <cmath>

ID3DXFont* Renderer::g_font = nullptr;
IDirect3DDevice9* Renderer::g_dev = nullptr;

void Renderer::Init(IDirect3DDevice9* dev) {
    g_dev = dev;
    if (!g_font)
        D3DXCreateFont(dev, 16, 8, FW_BOLD, 1, 0, DEFAULT_CHARSET,
                       OUT_DEFAULT_PRECIS, ANTIALIASED, VARIABLE_PITCH | FF_DONTCARE,
                       L"Consolas", &g_font);
}

ID3DXFont* Renderer::GetFont() { return g_font; }

bool Renderer::WorldToScreen(const G::Cam& cam, const float* world, D3DXVECTOR2* out) {
    D3DXMATRIX view, rotY, rotX, proj;
    D3DXMatrixRotationY(&rotY, cam.yaw);
    D3DXMatrixRotationX(&rotX, cam.pitch);
    D3DXMatrixMultiply(&view, &rotX, &rotY, &view);
    view._41 = -cam.pos[0]; view._42 = -cam.pos[1]; view._43 = -cam.pos[2];

    float aspect = 16.0f / 9.0f;
    D3DXMatrixPerspectiveFovLH(&proj, cam.fov, aspect, 0.1f, 10000.0f);

    D3DXVECTOR3 v(world[0], world[1], world[2]);
    D3DXVECTOR3 viewPos;
    if (FAILED(D3DXVec3Transform(&viewPos, &v, &view))) return false;
    if (viewPos.z <= 0.0f) return false;

    D3DXVECTOR3 projPos;
    D3DXVec3TransformProjCoord(&projPos, &viewPos, &proj);

    RECT rc; GetClientRect(g_dev->GetWindowHandle(), &rc);
    float w = (float)(rc.right - rc.left), h = (float)(rc.bottom - rc.top);
    out->x = (projPos.x + 1.0f) * 0.5f * w;
    out->y = (1.0f - projPos.y) * 0.5f * h;
    return true;
}
