// D2Game 0x0FFC0 -- from the cached decompile. Gold-vs-item-pick dispatcher: ECX/stack game, EDX itemType,
// ESI seed, EDI level. Seams in retail argument order; "ebx" is the caller-live value the callees read.
#include <cstdint>
#include <emscripten.h>

struct D2GameFFC0Env {
    int32_t (*o10814)(uint32_t tag);                                     // D2Common #10814 (cached once)
    int32_t (*sFA30)(int32_t game, uint64_t* seed, int32_t level, int32_t p3, int32_t ebx);
    int32_t (*sF8F0)(int32_t game, uint64_t* seed, int32_t level, int32_t p3, int32_t ebx);
    int32_t (*sF780)(int32_t game, uint64_t* seed, int32_t level, int32_t p2, int32_t p3, int32_t p4);
    void (*fatal)(uint32_t line);                                        // Ordinal_10265/10024 + _exit(-1)
    int32_t* cache;                                                      // DAT_6fd39fa0; 0 = uncached
    int32_t* cached;                                                     // DAT_6fd39fa4 bit 0
};

// 1.13c D2Game+0x0FFC0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_0FFC0(int32_t game, int32_t p2, int32_t p3, int32_t p4,
                                                          uint64_t* seed, int32_t level, int32_t ebx,
                                                          const D2GameFFC0Env* e)
{
    if (!(*e->cached & 1)) { *e->cached |= 1; *e->cache = e->o10814(0x20646c67); }
    const int32_t gld = *e->cache;
    if (gld == -1) { e->fatal(0x17c); return 0; }
    if (gld == 0)  { e->fatal(0x17f); return 0; }
    if (level >= 0x42) { e->fatal(0x180); return 0; }
    const int32_t half = level >> 1;
    int32_t a = half + 10;
    if (level & 1) a = half + 11;
    const int32_t b = half + 5 + (0x41 - level);
    *seed = (uint64_t)(uint32_t)*seed * 0x6ac690c5u + (uint32_t)(*seed >> 32);
    const int32_t r = (int32_t)((uint32_t)*seed % 100u);
    if (r < 0x41 - level) return gld;
    if (r < b)     return e->sFA30(game, seed, level, p3, ebx);
    if (r < b + a) return e->sF8F0(game, seed, level, p3, ebx);
    if (r < 100)   return e->sF780(game, seed, level, p2, p3, p4);
    e->fatal(0x1ba); return 0;
}
