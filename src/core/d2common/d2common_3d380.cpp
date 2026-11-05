// 1.13c D2Common+0x3D380 (#10485): stdcall(owner, list), ret 8. Null owner (0x10B), null list (0x10C) and a count byte of 8 or more (0x112, checked after the clear)
// are Fog fatal exits in retail (message through 0x9216, then exit(-1)); here they reach env->fatal. Otherwise: 0x3D250 clears the old text list (EDI = owner), then
// for k below the byte at list+0 (reread every pass): head = owner[+8], pool = owner[+0]; 0x9222(pool, 0xC, Text.cpp, line 0x5A, 0) returns a node;
// node u16 at +0 = the u16 at list + 4 + 4k, node dword at +4 = the byte at list + 2 + 4k, node dword at +8 = head; owner u16 at +4 gains 1 and owner[+8] = node.
#include <cstdint>
#include <cstring>

struct D2Common3D380Env {
    void (*clear)(uint32_t owner);                 // 0x3D250, EDI = owner
    uint32_t (*alloc)(uint32_t pool);              // 0x9222, ECX = pool, EDX = 0xC
    void (*fatal)(uint32_t code);                  // 0x10B / 0x10C / 0x112
};

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline uint32_t Rd16(uint32_t a) { uint16_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }
inline uint32_t Rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
inline void Wr(uint32_t a, uint32_t v) { memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
inline void Wr16(uint32_t a, uint32_t v) { const uint16_t w = static_cast<uint16_t>(v); memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &w, 2); }
}

extern "C" void D2Common_3D380_Raw(uint32_t owner, uint32_t list, const D2Common3D380Env* env)
{
    if (owner == 0) { env->fatal(0x10B); return; }
    if (list == 0) { env->fatal(0x10C); return; }
    env->clear(owner);
    if (Rd8(list) >= 8) { env->fatal(0x112); return; }
    for (uint32_t k = 0; k < Rd8(list); k++) {
        const uint32_t head = Rd(owner + 8);
        const uint32_t node = env->alloc(Rd(owner));
        Wr(node + 8, head);
        Wr16(node, Rd16(list + 4 + 4 * k));
        Wr(node + 4, Rd8(list + 2 + 4 * k));
        Wr16(owner + 4, Rd16(owner + 4) + 1);
        Wr(owner + 8, node);
    }
}
