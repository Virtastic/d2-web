// src/core/d2game/d2game_fb70.cpp -- 1.13c D2Game+0x0FB70.
// ECX = game, EAX = room-list index; plain ret. For every room of the list at game+0xbc+index*4, every unit of type 4
// (item) whose data record has a non-zero expiry (+0x24) at or before the game tick (game+0xa8) is released: when the
// owner lookup 0xA478 answers, the owner is told (0xA44E: owner, type, id) and the item is detached (0xA550, 0xA63A);
// then the item flag 2 (+0xC4) is cleared and the unit is passed to 0xDFC10 (EBX = 0x6FC2E7E0, stack = unit) and 0xE20A0 (ECX = game, EDX = unit).
#include <cstdint>
#include <cstring>
#include "d2game_seam.h"
static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
// 1.13c D2Game+0x0FB70
extern "C" void D2Game_sub_FB70(const D2GameSeam& S, uint32_t game, uint32_t index)
{
    const uint32_t list = rd32(game, 0xbc + index * 4);
    if (!list) return;
    const uint32_t tick = rd32(game, 0xa8);
    for (uint32_t room = S(0xA5C8, 0, 0, 0, 0, 0, list); room; room = rd32(room, 0x7c)) {
        uint32_t next;
        for (uint32_t u = rd32(room, 0x74); u; u = next) {
            next = rd32(u, 0xe8);
            if (rd32(u, 0) != 4) continue;
            const uint32_t exp = rd32(rd32(u, 0x14), 0x24);
            if (!exp || (int32_t)exp > (int32_t)tick) continue;
            const uint32_t owner = S(0xA478, 0, 0, 0, 0, 0, u);
            if (owner) {
                S(0xA44E, 0, 0, 0, 0, 0, owner, rd32(u, 0), rd32(u, 0xc));
                S(0xA550, 0, 0, 0, 0, 0, u);
                S(0xA63A, 0, 0, 0, 0, 0, u);
            }
            uint32_t fl = rd32(u, 0xc4) & 0xfffffffdu;
            std::memcpy((uint8_t*)(uintptr_t)(u + 0xc4), &fl, 4);
            S(0xDFC10, 0, 0, 0, 0x6FC2E7E0u, 0, u);
            S(0xE20A0, 0, game, u);
        }
    }
}
