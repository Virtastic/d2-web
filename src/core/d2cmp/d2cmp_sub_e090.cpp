// D2CMP cel-context open: direction scale, then the sprite-cache entry build. Retail: EAX = name, ECX = context, stack (s1, count,
// header, path, alt), ret 0x14. Context, header and strings are 32-bit retail addresses (flat guest image); the in-module callees are seams.
// Context: +4 flags (byte), +8 mode, +0x10 kind, +0x40 direction scale. Header: +0x14 directions.
#include <cstdint>
#include <cstring>

struct D2CmpE090Env {
    uint32_t g87f00;                                                                       // DAT 0x77F00
    uint32_t (*scale)(uint32_t a, uint32_t b);                                             // 0x12190: stack (count, 2*count), ret 8
    uint32_t (*open)(uint32_t hdr, uint32_t* count, uint32_t* s1, uint32_t ctx);           // 0x17020: ECX hdr, EDX &count, EDI &s1, ESI ctx, stack 0x6FE772B0
    uint32_t (*name)(uint32_t str, uint32_t ctx);                                          // 0xC670: EAX str, EDX ctx
    uint32_t (*path)(uint32_t s4, uint32_t str, uint32_t ctx);                             // 0xC2D0: stack s4 (ret 4), EBX str, ESI ctx
    uint32_t (*build)(uint32_t alt, uint32_t ctx, uint32_t hdr, uint32_t one, uint32_t count, uint32_t s4);   // alt ? 0xDC80 : 0xDDC0, stdcall ret 0x14
    void (*fatal)(uint32_t code);                                                          // Fog 10265 / 10024 / _exit(-1)
};

static inline uint32_t rdk(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t rdb(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }

// 1.13c D2CMP+0x0E090
uint32_t d2cmp_e090(uint32_t str, uint32_t ctx, uint32_t s1, uint32_t count, uint32_t hdr, uint32_t s4, uint32_t alt, const D2CmpE090Env* e)
{
    if (!hdr || !ctx) return str;
    const uint32_t m = rdk(ctx + 8), f = rdb(ctx + 4), k = rdk(ctx + 0x10);
    const bool gate = m != 0 || ((f & 8) && k && k != 0x11 && !e->g87f00);
    if (s1) return s1;
    if (count <= 1 || !gate) return 0;
    const uint32_t dirs = rdk(hdr + 0x14);
    const uint32_t sc = dirs > 1 ? (e->scale(count, count * 2) << 6) / dirs : 1;
    memcpy((void*)(uintptr_t)(ctx + 0x40), &sc, 4);
    uint32_t one = s1, n = count;
    const uint32_t r = e->open(hdr, &n, &one, ctx);
    if (r) return r;
    if (one != 1) { e->fatal(0x6ad); return 0; }
    if (n <= 1) { e->fatal(0x6ae); return 0; }
    if (!rdb(str) && !e->name(str, ctx)) return 0;
    if (!rdb(s4) && !e->path(s4, str, ctx)) return 0;
    return e->build(alt, ctx, hdr, 1, n, s4);
}
