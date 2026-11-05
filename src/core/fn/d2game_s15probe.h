// Shared probe glue of struct batch 15 (d2game f1730, f17e0, f29b0): D2Seam thunks over HookEnv plus the ten-stack-word 0xF14F0 callee.
// Blobs lg 0, sc 1 (the replay table of (key, value) pairs; hookw out-writes ride as ((1+word)<<24)|rva).
#pragma once
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "d2game_hk2.h"
#include "../d2game/d2game_s15.h"
namespace s15 {
inline uint32_t thunk(void* c, uint32_t rva, uint32_t a, uint32_t cc, uint32_t d, uint32_t b, uint32_t s, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3,
                      uint32_t s4, uint32_t s5, uint32_t s6, uint32_t) {
    return ((HookEnv*)c)->h(rva, a, cc, d, b, s, s0, s1, s2, s3, s4, s5, s6);
}
inline void blkthunk(void* c, uint32_t rva, uint32_t p, uint32_t n) { ((HookEnv*)c)->blk(rva, (const uint32_t*)(uintptr_t)p, n); }
inline void blkmthunk(void* c, uint32_t rva, const uint32_t* p, uint32_t n, uint32_t mask) { ((HookEnv*)c)->blk(rva, p, n, mask); }
// 0xF14F0: record (EAX, ECX, words 0-3), words 5-6 written by h(), then the full ten-word block, then the writes through words 7-8 (h() reaches only 0-6)
inline uint32_t f14(void* c, uint32_t eax, uint32_t ecx, uint32_t* w) {
    HookEnv* e = (HookEnv*)c;
    const uint32_t rv = e->h(0xF14F0, eax, ecx, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6]);
    e->blk(0xF14F0, w, 10);
    for (uint32_t i = 7; i <= 8; i++) {
        const uint32_t key = ((1u + i) << 24) | 0xF14F0; uint32_t off = 0;
        for (const uint32_t* p = e->sc; p[0]; p += 2) if (p[0] == key) { HookEnv::wr32(w[i] + off, p[1]); off += 4; }
    }
    return rv;
}
}
