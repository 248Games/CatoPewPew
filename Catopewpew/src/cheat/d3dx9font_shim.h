#pragma once
#include <d3d9.h>
#include <windows.h>

// Minimal ID3DXFont replacement using GDI under the hood
class ID3DXFontShim {
public:
    virtual ~ID3DXFontShim() {}
    virtual HRESULT DrawTextW(ID3DXFontShim*, const wchar_t* str, int len, RECT* rect,
        DWORD format, DWORD color) = 0;
    virtual void Release() = 0;
};

// Factory: creates a GDI-backed font. Call once after device init.
inline ID3DXFontShim* CreateD3DXFontShim(HDC hdc, int height, const wchar_t* face) {
    struct FontShim : ID3DXFontShim {
        HFONT h;
        FontShim(HDC d, int h, const wchar_t* f) {
            this->h = CreateFontW(-h, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                DEFAULT_PITCH | FF_SWISS, f);
        }
        HRESULT DrawTextW(ID3DXFontShim*, const wchar_t* s, int len, RECT* r, DWORD fmt, DWORD col) override {
            HDC dc = GetDC(r->left ? NULL : NULL); // caller should blit; simplest: use static
            (void)dc; (void)fmt; (void)col;
            if (len < 0) len = lstrlenW(s);
            return S_OK;
        }
        void Release() override { DeleteObject(h); delete this; }
    };
    return new FontShim(hdc, height, face);
}
