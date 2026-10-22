// D2Game 0x0FC40 -- from the cached decompile plus the disassembly (register/stack arguments resolved).
// Retail args: EAX = unit, ECX = level, stack = (src, seed, unit2, resist[4]). Returns a result code 0..7.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint8_t  rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2GameFC40Env {
    const uint8_t* recs;   // *(sgptDataTables+0xbf8), stride 0xe4
    int32_t nrecs;         // *(sgptDataTables+0xbfc)
    void*   (*sA53E)(void* unit);                               // D2Common #10695
    int32_t (*sB444)(void* unit);                               // D2Common #10082
    const int32_t* (*sB42C)(void* unit, uint32_t b, int32_t c); // D2Common #10560; null is retail's fatal
    int32_t (*s2F2F0)(void* unit2);                             // 0xF2F0 (ESI)
    int32_t (*s2E130)(int32_t a, int32_t b, int32_t c);         // 0xE130
    uint32_t (*s211D0)(void* seedp);                            // 0x11D0 (ECX = seed + 0x20)
    void (*fatal)();                                            // Fog log + _exit(-1)
};

// 1.13c D2Game+0x0FC40
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_0FC40(void* unit, int32_t level, const void* src, uint8_t* seed,
                                                           void* unit2, const uint16_t* res, const D2GameFC40Env* e)
{
    void* info = e->sA53E(unit);
    if (!info) return 0;
    int32_t idx = e->sB444(unit);
    if (idx < 0 || idx >= e->nrecs) return 0;
    const uint8_t* rec = e->recs + (uint32_t)idx * 0xe4;
    if (rd8(rec, 0x16)) return 2;
    if (rd8(info, 0x129) || (rd8(rec, 0x14) && rd8(info, 0x12a))) return 7;
    const int32_t* t = e->sB42C(unit, rd8(src, 0x6d), 0x64);
    if (!t) { e->fatal(); return 0; }
    const int32_t d = level - rd8(info, 0xfd);
    int32_t ebx = e->s2F2F0(unit2);
    int flag = 0;
    bool skipTo3 = false;
    if (ebx) { if (ebx <= -100) skipTo3 = true; else flag = 1; }
    if (!skipTo3) ebx += 100;
    auto calc = [&](int a, int b, int c, int ri_, int scale) -> int32_t {
        int32_t s = (t[a] - d / t[b]) * 0x80;
        if (flag) {
            int32_t m = scale ? e->s2E130(ebx, scale, scale) : ebx;
            if (m) s = (int32_t)((uint32_t)s * 100u) / m;
        }
        if (s <= t[c]) s = t[c];
        int32_t p = (int32_t)((uint32_t)rd16(res, ri_ * 2) * (uint32_t)s);
        return s - ((p + ((p >> 31) & 0x3ff)) >> 10);
    };
    if (!skipTo3) {
        if (calc(0, 1, 2, 3, 0xfa) <= 0 || e->s211D0(seed + 0x20) < 0x80) return 7;
        if (calc(6, 7, 8, 2, 0x1f4) <= 0 || e->s211D0(seed + 0x20) < 0x80) return 5;
        if (rd8(rec, 0x15) && (calc(3, 4, 5, 1, 0x258) <= 0 || e->s211D0(seed + 0x20) < 0x80)) return 6;
        if (rd8(rec, 0x14)) return 4;
        if (calc(9, 10, 11, 0, 0) <= 0 || e->s211D0(seed + 0x20) < 0x80) return 4;
    }
    if ((t[12] - d / t[13]) * 0x80 <= 0 || e->s211D0(seed + 0x20) < 0x80) return 3;
    if ((t[14] - d / t[15]) * 0x80 <= 0) return 2;
    return (e->s211D0(seed + 0x20) < 0x80) + 1;
}
