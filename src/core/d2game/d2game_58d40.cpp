// src/core/d2game/d2game_58d40.cpp -- 1.13c D2Game.dll sub_58D40 from the cached decompile.
// Same quest-state callback shape as 0x58F90 (quest id 0x28); callees are seams on the 58F90 env.
#include <cstdint>
#include <cstring>

struct D2GameBatch58F90Env {
    void*   (*ordinal10920)(void* p);
    uint32_t(*ordinal10174)(uint32_t a, uint32_t b, uint32_t c);
    void    (*sub354F0)(void* a, void* b);
};
extern D2GameBatch58F90Env g_d2game58F90Env;

// 1.13c D2Game+0x58D40
uint32_t d2game_58D40(void* p1, void* p2) {
    auto& E = g_d2game58F90Env;
    void* r = E.ordinal10920(p2);
    uint32_t u; std::memcpy(&u, (uint8_t*)r + 0x10 + ((uint8_t*)p1)[0x6d] * 4, 4);
    if ((E.ordinal10174(u, 0x28, 0) != 0 || E.ordinal10174(u, 0x28, 0xf) != 0) &&
        E.ordinal10174(u, 0x28, 0xd) == 0 && E.ordinal10174(u, 0x28, 0xe) == 0) return 0;
    E.sub354F0(p2, p1);
    return 0;
}
