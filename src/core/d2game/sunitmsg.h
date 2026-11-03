// Shared by the SUnitMsg queue appenders (D2Game 1.13c +0x7A020, 7A090, 7A440, 7A4C0). Register convention: ESI = the owner block;
// the seams are the two import thunks the appenders call: 0xA2E6 (Fog alloc: ECX = owner+8 or 0, EDX = size, stack file/line/0) and
// 0xA4DE (stdcall, one stack word = owner). Addresses are 32-bit words as in the FN probes.
#pragma once
#include <cstdint>
#include <cstring>
struct SUnitMsgSeams {
    uint32_t (*alloc)(uint32_t ecx, uint32_t size, uint32_t line);
    void     (*signal)(uint32_t owner);
};
namespace sunitmsg {
inline uint32_t r32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
inline void w32(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }
inline void w16(uint32_t p, uint32_t o, uint32_t v) { uint16_t x = (uint16_t)v; std::memcpy((void*)(uintptr_t)(p + o), &x, 2); }
inline void w8(uint32_t p, uint32_t o, uint32_t v) { *(uint8_t*)(uintptr_t)(p + o) = (uint8_t)v; }
// allocate a message of `size` bytes charged to the owner's pool (owner+8)
inline uint32_t alloc(const SUnitMsgSeams& s, uint32_t owner, uint32_t size, uint32_t line) {
    return s.alloc(owner ? r32(owner, 8) : 0, size, line);
}
// append to the owner's singly-linked queue: head at +0xEC (set when empty), tail at +0xF0 (old tail->next = m), then signal
inline void enqueue(const SUnitMsgSeams& s, uint32_t owner, uint32_t m) {
    if (r32(owner, 0xec) == 0) w32(owner, 0xec, m);
    if (const uint32_t t = r32(owner, 0xf0)) w32(t, 0, m);
    w32(owner, 0xf0, m);
    s.signal(owner);
}
}
