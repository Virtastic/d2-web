// 1.13c D2Game+0x2CD30 (236 B, EAX = game, plain ret; ECX = GUID is only copied to ESI, which the body overwrites): tears one game down. Fifteen
// subsystem teardowns run in this order (all plain-ret, register-passed): 0x2A0D0 (EAX = game), 0xCA3B0 (EAX = game+0xF0), 0xB8A0, 0x33DC0,
// 0x90450 (EBX = game), 0x49E90, 0xB0640, 0x33290, 0x38860, 0x13180, 0x6FEC0, 0xE21F0, 0x1ED80 (EAX = game), 0x8E4D0, 0x9BD90. Then a non-zero
// +0x1D28 is released (Fog #10046 with ECX = +0x1C, EDX = that value, file Arena.cpp, line 0x40) and cleared; the five dwords from +0xBC are
// each freed through Fog #10868 and cleared; a non-zero +0x1C is released through Fog #10143 (cdecl); the lock (+0x18) is left and deleted
// (DeleteCriticalSection) and cleared, and Fog #10046 (ECX = 0, EDX = the lock, Game.cpp, line 0x888) frees it.
#include "d2game_gamelist.h"

struct D2Game2CD30Env {
    void (*teardown)(uint32_t nStep, void* pGame);                      // the 15 callees above, nStep 0..14 in call order
    void (*free46)(uint32_t nPool, uint32_t nPtr, uint32_t nLine, bool bArena);   // Fog #10046 (0xA2EC)
    void (*free10868)(uint32_t nPtr);                                   // 0xB32A
    void (*free10143)(uint32_t nPool);                                  // 0xA340 (cdecl)
    void (*leaveCs)(void* pCs);
    void (*deleteCs)(void* pCs);
};

// 1.13c D2Game+0x2CD30
void D2Game_2CD30(void* pGame, const D2Game2CD30Env* pEnv)
{
    uint8_t* g = static_cast<uint8_t*>(pGame);
    for (uint32_t i = 0; i < 15; ++i) pEnv->teardown(i, pGame);
    if (d2gl::rd32(g, 0x1D28) != 0) {
        pEnv->free46(d2gl::rd32(g, 0x1C), d2gl::rd32(g, 0x1D28), 0x40, true);
        const uint32_t z = 0; std::memcpy(g + 0x1D28, &z, 4);
    }
    for (uint32_t i = 0; i < 5; ++i) {
        const uint32_t v = d2gl::rd32(g, 0xBC + 4 * i);
        if (v != 0) { pEnv->free10868(v); const uint32_t z = 0; std::memcpy(g + 0xBC + 4 * i, &z, 4); }
    }
    if (d2gl::rd32(g, 0x1C) != 0) pEnv->free10143(d2gl::rd32(g, 0x1C));
    void* pCs = d2gl::rdptr(g, 0x18);
    pEnv->leaveCs(pCs);
    const uint32_t z = 0; std::memcpy(g + 0x18, &z, 4);
    pEnv->deleteCs(pCs);
    pEnv->free46(0, reinterpret_cast<uintptr_t>(pCs), 0x888, false);
}
