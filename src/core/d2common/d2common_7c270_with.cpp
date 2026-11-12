// 1.13c D2Common+0x7C270 with its three callees as an env seam (EAX = room, retail-layout 32-bit tokens read raw). The FN probe
// src/core/fn/d2common_7c270.cpp routes the callees through hook recorders; production sub_7C270 (d2common_batch_b62.cpp) keeps
// the native seams. Three lazy steps gated by the room flags at +0x28, each flag re-read after the previous call.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Common7C270Env {
    void (*loadDt1)(void* pRoom);
    void (*spawnPresets)(void* pRoom);
    void (*postInit)(void* pRoom, uint32_t nEcx);
};

namespace {
uint32_t T32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

EMSCRIPTEN_KEEPALIVE uint32_t D2Common_7C270_With(void* pUnitEAX, const D2Common7C270Env* e) {
    const uint32_t room = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pUnitEAX));
    if (!(T32(room + 0x28) & 0x1000000)) e->loadDt1(pUnitEAX);
    if (!(T32(room + 0x28) & 0x2000000) && T32(room + 0x48) == 2) e->spawnPresets(pUnitEAX);
    if (!(T32(room + 0x28) & 0x100000)) e->postInit(pUnitEAX, T32(T32(T32(room + 0x58) + 0x1b4) + 0x478));
    return T32(room + 0x30);
}
