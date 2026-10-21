// D2Common 1.13c batch: relink a unit into its owner's list (state image keeps 32-bit tokens).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Common7bc60Env {
    void* (*resolve)(uint32_t token);   // retail address -> host pointer
    uint32_t (*tokenOf)(const void* p); // host pointer -> retail address
};

namespace {
uint32_t rd(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
void wr(void* p, uint32_t o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }
}

extern "C" {
// 1.13c D2Common+0x7BC60: unit (EAX) at +0x44 kind byte; kind 3 = already linked, return.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7bc60(uint8_t* u, const D2Common7bc60Env* env) {
    if (u[0x44] == 3) return;
    // Unlink from the current pair (+0xE8 prev / +0x1C next), if both are set.
    if (rd(u, 0xE8) && rd(u, 0x1C)) {
        wr(env->resolve(rd(u, 0xE8)), 0x1C, rd(u, 0x1C));
        wr(env->resolve(rd(u, 0x1C)), 0xE8, rd(u, 0xE8));
        wr(u, 0xE8, 0);
        wr(u, 0x1C, 0);
    }
    // Retail oddity kept: owner = *(*(u+0x58)+0x1B4) with no null check; tail sentinel at owner+0x364.
    uint8_t* owner = static_cast<uint8_t*>(env->resolve(rd(env->resolve(rd(u, 0x58)), 0x1B4)));
    const uint32_t self = env->tokenOf(u);
    wr(u, 0x1C, env->tokenOf(owner + 0x364));
    const uint32_t tail = rd(owner, 0x44C);
    wr(u, 0xE8, tail);
    wr(env->resolve(tail), 0x1C, self);
    wr(owner, 0x44C, self);
    u[0x44] = 3;
}
}
