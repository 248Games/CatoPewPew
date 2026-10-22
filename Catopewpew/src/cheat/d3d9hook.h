#pragma once
#include <d3d9.h>      // gives us D3DMATRIX, D3DXVECTOR2/3/4 for free
#include <cmath>

// Only add what d3d9.h does NOT provide:
inline D3DXVECTOR3 D3DXVec3Cross(const D3DXVECTOR3* o, const D3DXVECTOR3& a, const D3DXVECTOR3& b) {
    *o = D3DXVECTOR3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); return *o;
}
inline float D3DXVec3Dot(const D3DXVECTOR3& a, const D3DXVECTOR3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline void D3DXMatrixIdentity(D3DMATRIX m) {
    for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] = (i == j) ? 1.f : 0.f;
}
