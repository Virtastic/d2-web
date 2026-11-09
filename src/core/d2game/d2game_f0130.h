// 1.13c D2Game+0xF0130: EAX = unit, EDI = player. Sends the relation-state packet (0x8B, 6 bytes: id of the EAX unit, low byte of the
// relation node's state) to the player's client when a node for the EAX unit exists in the player's relation list and the two ids differ.
// pd = #10920(EDI) through thunk 0xA4C0 (stdcall 1); id = unit ? unit+0xC : -1; the list at pd+0x38 (next at +0x10) is walked only when EDI != EAX.
// client = #10920(EDI)+0x9C when EDI is a type-0 unit, else 0; the packet goes out through 0x8A3E0 (EAX = client, stack &packet, 6).
#pragma once
#include <cstdint>
#include <cstring>
namespace gf0130 {
template <class E, class Send> void Run(E& e, Send& send, uint32_t eax, uint32_t edi)
{
    // 1.13c D2Game+0xF0130
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, edi);
    const uint32_t id = eax ? e.rd32(eax + 0xC) : 0xFFFFFFFFu;
    uint32_t node = 0;
    if (edi != eax)
        for (uint32_t n = e.rd32(pd + 0x38); n; n = e.rd32(n + 0x10))
            if (e.rd32(n) == id) { node = n; break; }
    const uint32_t pid = edi ? e.rd32(edi + 0xC) : 0xFFFFFFFFu;
    if (pid == id || !node) return;
    uint32_t client = 0;
    if (edi && e.rd32(edi) == 0) client = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, edi) + 0x9C);
    uint8_t p[8] = { 0x8B, 0, 0, 0, 0, (uint8_t)e.rd32(node + 0xC), 0, 0 };
    std::memcpy(p + 1, &id, 4);
    send(client, p, 6u);
}
}
