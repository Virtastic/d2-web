// 1.13c D2Client+0x83880: item packet -- an item is placed into another unit's inventory. Retail words (units, the inventory
// handle) are 32-bit addresses; callees are the thunk RVAs named per member.
#pragma once
#include <cstdint>
struct D2c83880G {
    uint32_t* player;   // +0x11BBFC  player unit (retail word)
    uint32_t* hash;     // +0x10AE08  item unit hash, 128 buckets, chain at unit+0xE4, id at +0xC, type at +0
};
struct D2c83880Env {
    void     (*ca90)(uint32_t src, uint32_t n, uint32_t zero, uint8_t* out);                   // bit-stream unpack into the 24-byte local
    uint32_t (*fa4e20)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0);                 // unit lookup: EAX id&0x7f, ECX id, EDX bucket base; type
    void     (*f82820)(uint32_t eax, uint32_t edi, uint32_t s0, uint32_t s1, uint32_t s2);     // EAX byte L+8, EDI 0; words L+4, L+6, 0
    void     (*ca24)(uint32_t item, uint32_t v);                                               // v: dword whose low byte is L+0x10 (upper bytes stack residue)
    uint32_t (*f833f0)(uint32_t ebx, const uint8_t* pkt, uint8_t* L, uint32_t s2, uint32_t s3); // build the item unit from the packet
    uint32_t (*ca6c)(uint32_t inv, uint32_t item, uint32_t one);
    uint32_t (*ca3c)(uint32_t v, uint32_t mask);
    void     (*c7d8)(uint32_t inv, uint32_t item);
    void     (*c202)(uint32_t inv);
    void     (*f162d0)();
    void     (*f81ff0)(uint32_t edi, uint32_t esi, uint32_t s0);                               // EDI owner, ESI item
    void     (*f81aa0)(uint32_t esi);                                                          // ESI item
    void     (*fatal)(uint32_t code);                                                          // 10265/10024 + _exit(-1)
};
extern "C" void d2_client_83880(D2c83880G* g, const D2c83880Env* e, const uint8_t* pkt);
