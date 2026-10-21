// D2Game 1.13c 0x6A0D0 -- client state notice: sends packet 0x0E {02, client id, flag, ushort +0x10, 03}
// (12 bytes), then by the class record flags (D2Common #10688 +0x167): bit2 -> 0x8AAE0, else for a type-1
// client with bit0, a second 17-byte packet carrying the #10092 byte and the unit-owner id - 1.
// Register-only args are explicit: client = in_EAX. #10688/#10092 take their args in registers; seams
// here are argument-less like the sibling 0x6B200 env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; memcpy(&v, (const uint8_t*)p + o, 2); return v; }
struct D2Game6A0D0Env {
    void           (*send8A3E0)(const void* pkt, uint32_t len);
    const uint8_t* (*ordinal10688)();
    const uint8_t* (*ordinal10092)();
    void           (*sub8AAE0)();
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_6a0d0(const uint8_t* client, const D2Game6A0D0Env* env) { // 1.13c D2Game+0x6A0D0
    uint8_t p[17] = {};  // one buffer, as in the decompile frame
    uint32_t id = 0xFFFFFFFFu; uint8_t f = 0;
    if (client) { id = rd32(client, 0xC); f = (rd32(client, 0xC4) >> 1) & 1; }
    p[0] = 0x0E; p[1] = 2; memcpy(p + 2, &id, 4); p[6] = 3; p[7] = f;
    uint32_t t = rd16(client, 0x10); memcpy(p + 8, &t, 4);
    env->send8A3E0(p, 12);
    uint8_t b = env->ordinal10688()[0x167];
    if (!b) return;
    if (b & 4) { env->sub8AAE0(); return; }
    if (rd32(client, 0x10) == 1 && (b & 1)) {
        const uint8_t* q = env->ordinal10092();
        uint8_t v = q ? *q : 0;
        uint32_t o = rd32((const void*)(uintptr_t)rd32(client, 0x14), 0xC);
        if (o) {   // ponytail: decompile frame for the 17-byte send is lossy (buffer at stack0xffffffe4, bytes 12..16 unnamed); zero-filled here
            o -= 1;
            p[0] = (uint8_t)(o >> 16); p[1] = (uint8_t)(o >> 24); p[2] = v;
            p[3] = p[4] = p[5] = p[6] = p[7] = p[8] = 0;
            env->send8A3E0(p, 17);
        }
    }
}
