// 1.13c D2Client+0x83790: item packet -- an item lands on another unit's cursor. Retail words (units, the inventory handle) are
// 32-bit addresses; callees are the thunk RVAs named per member.
#pragma once
#include <cstdint>
struct D2c83790G {
    uint32_t* b834;     // +0x11B834  cursor item handle (C202 result)
    uint32_t* b854;     // +0x11B854
    uint32_t* b858;     // +0x11B858
    uint32_t* b864;     // +0x11B864
};
struct D2c83790Env {
    void     (*ca90)(uint32_t src, uint32_t n, uint32_t zero, uint8_t* out);                   // bit-stream unpack into the 24-byte local
    uint32_t (*fa4e20)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0);                 // unit lookup: EAX id&0x7f, ECX id, EDX bucket base; type
    void     (*a6ef0)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1);                   // unhash: ECX item hash, EDX id&0x7f; id, type 4
    uint32_t (*f833f0)(uint32_t ebx, const uint8_t* pkt, uint8_t* L, uint32_t s2, uint32_t s3); // build the item unit from the packet
    void     (*c40c)(uint32_t item, uint32_t a, uint32_t b, uint32_t c);
    void     (*c7d8)(uint32_t inv, uint32_t item);
    uint32_t (*c202)(uint32_t inv);
    void     (*fatal)(uint32_t code);                                                          // 10265/10024 + _exit(-1)
};
extern "C" void d2_client_83790(D2c83790G* g, const D2c83790Env* e, const uint8_t* pkt);
