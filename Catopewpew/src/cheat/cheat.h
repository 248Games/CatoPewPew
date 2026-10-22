#pragma once
#include "game.h"
#include <vector>

struct Settings {
    bool aimbot = true;
    bool esp = true;
    bool fovCircle = true;
    bool triggerBot = false;
    float aimSmooth = 0.25f;   // 0..1, lower = snappier
    int   fovRadius = 180;     // pixels
    int   maxDist = 300;       // world units
    bool  menuOpen = false;
};

namespace Cheat {
    void Init();
    void Frame(LPDIRECT3DDEVICE9 dev);   // called each Present
    const Settings& GetSettings();
}
