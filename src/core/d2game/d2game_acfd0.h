// Item slot packet: ECX unused, EDX = player, stack = item, 3 unused words, tail (ret 0x14); result 1. #A65E(item, 4, 1); slot = #ADC6(item);
// a slot >= 0 sends the 8-byte packet {0x3F, slot, item ? item[3] : 0 (dword), tail (word)} by 0x8A3E0(EAX = client; &packet, 8), the client
// being #A4C0(player)+0x9C for a non-null type-0 player and 0 otherwise.
#pragma once
#include <cstdint>

namespace gacfd0 {
template <class E> uint32_t Send(E& e, uint32_t player, uint32_t item, uint32_t tail)
{
    // 1.13c D2Game+0xACFD0
    e.h(0xA65E, 0, 0, 0, 0, 0, item, 4, 1);
    const int32_t slot = (int32_t)e.h(0xADC6, 0, 0, 0, 0, 0, item);
    if (slot < 0) return 1;
    const uint32_t client = player && e.rd32(player) == 0 ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9C) : 0;
    uint32_t pkt[2] = { 0, 0 };
    uint8_t* b = (uint8_t*)pkt;
    b[0] = 0x3F; b[1] = (uint8_t)slot;
    const uint32_t id = item ? e.rd32(item + 0xC) : 0;
    for (int i = 0; i < 4; i++) b[2 + i] = (uint8_t)(id >> (8 * i));
    b[6] = (uint8_t)tail; b[7] = (uint8_t)(tail >> 8);
    e.h(0x8A3E0, client, 0, 0, 0, 0, e.stk(pkt), 8);
    e.blk(0x8A3E0, pkt, 2);
    return 1;
}
}
