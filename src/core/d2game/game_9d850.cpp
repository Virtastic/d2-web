// 1.13c D2Game+0x9D850 (EBX = game): per act 0..4 with its pending flag set, run the queued units of every room, then clear the flag;
// finally clear bit 0x400 of the arena control's flags. Retail 32-bit layout; the room/unit walk is a seam.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#pragma pack(push, 1)
struct D2G9D850Game { uint8_t pad[0xBC]; uint32_t act[5]; uint8_t pad2[0x1D28 - 0xD0]; uint32_t arena; };
struct D2G9D850Arena { uint32_t pad0, pad4, flags; };
#pragma pack(pop)
static_assert(offsetof(D2G9D850Game, act) == 0xBC && offsetof(D2G9D850Game, arena) == 0x1D28 && offsetof(D2G9D850Arena, flags) == 8, "game fields");

// walkRooms(act): rooms (10717, next +0x7C), each queue (+0x1C, next +0xE0) through E0860(game, unit), then 10127(room).
template <class Ptr, class Walk> static void GAME_9D850(D2G9D850Game* g, Ptr p, Walk walkRooms)
{
    for (int i = 0; i < 5; ++i) {
        uint32_t* a = (uint32_t*)p(g->act[i]);
        if (a && *a) { walkRooms(a); *a = 0; }
    }
    if (g->arena) ((D2G9D850Arena*)p(g->arena))->flags &= 0xFFFFFBFFu;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9d850(uint32_t* r)
{
    // ponytail: the replay never sets an act flag (retail would walk real rooms), so the walk seam stays empty.
    GAME_9D850((D2G9D850Game*)(uintptr_t)r[0], [](uint32_t v) { return (void*)(uintptr_t)v; }, [](uint32_t*) {});
    r[4] = 0;
}
