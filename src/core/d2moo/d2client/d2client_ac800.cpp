// Server packet: tell a player unit a value. Retail: ECX = packet, plain ret. Nothing happens when packet byte +0xB is set.
// The unit id (dword at packet+3) selects bucket id & 0x7F of the 128-bucket unit table at D2Client+0x10A608; the chain is
// walked through the next link at node+0xE4 until node+0xC equals the id (no match: return). A node whose first dword (unit
// type) is not 0 is fatal (0x5A). Otherwise thunk 0xC4EA (ECX = node, EDX = word at packet+7, stack -1; ret 4) answers an
// object; zero is fatal (0xAD6), else thunk 0xC4E4(object, byte at packet+9) (stdcall, 2 words). Retail 32-bit layout.
// 1.13c D2Client+0xAC800
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cAc800Env {
    const uint32_t* table;                                         // D2Client+0x10A608, 128 node pointers
    uint32_t (*c4ea)(uint32_t node, uint32_t word, int32_t minus1);
    void (*c4e4)(uint32_t obj, uint32_t b);
    void (*fatal)(uint32_t code);                                  // Fog 10265 + Fog 10024 + _exit(-1)
};

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_ac800(const uint8_t* pk, const D2cAc800Env* env) {
    if (pk[0xb]) return;
    uint32_t id; std::memcpy(&id, pk + 3, 4);
    uint16_t w; std::memcpy(&w, pk + 7, 2);
    uint32_t n = env->table[id & 0x7f];
    if (!n) return;
    while (rd32(n + 0xc) != id) { n = rd32(n + 0xe4); if (!n) return; }
    if (rd32(n) != 0) { env->fatal(0x5a); return; }
    const uint32_t obj = env->c4ea(n, w, -1);
    if (!obj) { env->fatal(0xad6); return; }
    env->c4e4(obj, pk[9]);
}
