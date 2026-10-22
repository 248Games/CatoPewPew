#pragma once
#include <windows.h>
#include <d3d9.h>
#include <dxgitype.h>
#include <vector>
#include <string>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// Game-specific data. catopewpew is a placeholder title — tweak the offsets /
// struct layout below to match your actual game build. Everything else in the
// cheat (rendering, aim math, menu) is generic and works with any D3D9 game.
namespace G {
    // One entity as the game stores it in memory.
    struct Entity {
        float pos[3];      // world position (x, y, z)
        float headOffset;  // height of head above pos
        int   id;
        bool  isEnemy;
        bool  alive;
    };

    // Where the game keeps its data. Replace with real offsets from your build.
    constexpr uintptr_t ENT_LIST_PTR = 0x1A2B3C4D;  // *(int**)(base + ENT_LIST_PTR) = first entity
    constexpr int       MAX_ENTS     = 512;

    // Camera / view data (for world->screen projection).
    struct Cam {
        float pos[3];
        float fov;
        float yaw, pitch;   // radians
    };
    constexpr uintptr_t CAM_PTR = 0x1B2C3D4E;       // *(Cam*)(base + CAM_PTR)
}
