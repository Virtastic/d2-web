// Player item-slot refresh: stdcall(player), ret 4; result 0. With a list at player+0x60, the list is walked #A3DC(list) .. #A4CC(it);
// each entry's unit #A46C(it) that #A8A4(unit, 4, 0x78, file) accepts gets #A65E(unit, 4, 0) and the 8-byte packet
// {0x3F, 0xFF, unit ? unit[3] : 0 (dword), 0xFFFF} sent by 0x8A3E0(EAX = client; &packet, 8), the client being #A4C0(player)+0x9C for a
// type-0 player and 0 otherwise.
#pragma once
#include <cstdint>

namespace gacf10 {
static constexpr uint32_t kFile = 0xB20FB660u;   // the SkillItem.cpp-style source-file string the ordinal is handed (module address)
template <class E> uint32_t Refresh(E& e, uint32_t player)
{
    // 1.13c D2Game+0xACF10
    const uint32_t list = e.rd32(player + 0x60);
    if (!list) return 0;
    for (uint32_t it = e.h(0xA3DC, 0, 0, 0, 0, 0, list); it; it = e.h(0xA4CC, 0, 0, 0, 0, 0, it)) {
        const uint32_t unit = e.h(0xA46C, 0, 0, 0, 0, 0, it);
        if (!e.h(0xA8A4, 0, 0, 0, 0, 0, unit, 4, 0x78, kFile)) continue;
        e.h(0xA65E, 0, 0, 0, 0, 0, unit, 4, 0);
        const uint32_t client = e.rd32(player) == 0 ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9C) : 0;
        uint32_t pkt[2] = { 0, 0 };
        uint8_t* b = (uint8_t*)pkt;
        b[0] = 0x3F; b[1] = 0xFF;
        const uint32_t id = unit ? e.rd32(unit + 0xC) : 0;
        for (int i = 0; i < 4; i++) b[2 + i] = (uint8_t)(id >> (8 * i));
        b[6] = 0xFF; b[7] = 0xFF;
        e.h(0x8A3E0, client, 0, 0, 0, 0, e.stk(pkt), 8);
        e.blk(0x8A3E0, pkt, 2);
    }
    return 0;
}
}
