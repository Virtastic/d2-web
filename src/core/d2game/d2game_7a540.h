// Object-removal notice: ECX = the object record, one stack word = the client (ret 4). Unless the record's type byte (+0) is 3,
// a 6-byte packet {0x0A, type byte, the record's id (+0xC)} goes to the client's send step (EAX = client; stack the packet, 6).
#pragma once
#include <cstdint>

namespace d2g7a540 {
template <class E> void Run(E& e, uint32_t rec, uint32_t client)
{
    // 1.13c D2Game+0x7A540
    const uint32_t type = e.rd8(rec);
    if (type == 3) return;
    const uint32_t id = e.rd32(rec + 0xC);
    uint8_t pkt[6] = { 0x0A, (uint8_t)type, (uint8_t)id, (uint8_t)(id >> 8), (uint8_t)(id >> 16), (uint8_t)(id >> 24) };
    e.send(client, pkt, 6);
}
}
