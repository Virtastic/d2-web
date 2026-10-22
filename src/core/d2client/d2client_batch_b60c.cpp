// D2Client batch b60-cli-c: env-seam ports (globals/callees injected, no retail data).
// Skipped (register-arg / x87 / SEH / too large): 87BD0 87F30 88440 88A70 88DD0 89830 8A4D0 8A6C0.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;
static inline i32 rd(const void* p, u32 off) { i32 v; memcpy(&v, (const char*)p + off, 4); return v; }

extern "C" {
// 1.13c D2Client+0x87C30  list node = {i32* item, node* next}; slots are 0x8e bytes.
struct D2cNode60 { const uint8_t* item; D2cNode60* next; };
EMSCRIPTEN_KEEPALIVE const uint8_t* d2c_87c30(const D2cNode60* list, const uint8_t* slots, i32 slotCnt,
    i32 (*ord10053)(), i32 p1, i32 p2, const uint8_t* p3) {
    const uint8_t* best = nullptr;
    for (const D2cNode60* n = list; n; n = n->next) {
        const uint8_t* it = n->item;
        i32 i = rd(it, 4);
        const uint8_t* s = (!slots || i < 0 || slotCnt <= i) ? nullptr : slots + i * 0x8e;
        if (rd(s, 0x60) == p1 && it != p3 && rd(it, 0x30) == 1 && (p2 == 0 || ord10053() != 0) &&
            (!best || (u32)rd(it, 0x34) < (u32)rd(best, 0x34)))
            best = it;
    }
    return best;
}

// 1.13c D2Client+0x87D80  pool of 0x59-byte entries; ord10034 = tick/stamp call, seq = g_cbf38.
EMSCRIPTEN_KEEPALIVE uint8_t* d2c_87d80(uint8_t* pool, const uint8_t* poolEnd, i32* seq, void (*ord10034)()) {
    for (uint8_t* p = pool; p < poolEnd; p += 0x59) {
        if (rd(p, 0) == 0) {
            memset(p, 0, 0x59);
            i32 one = 1; memcpy(p, &one, 4);
            ord10034();
            i32 s = ++*seq; memcpy(p + 8, &s, 4);
            return p;
        }
    }
    return nullptr;
}

// 1.13c D2Client+0x88080  (edx = id)
EMSCRIPTEN_KEEPALIVE i32 d2c_88080(const D2cNode60* list, i32 id) {
    if (id) for (const D2cNode60* n = list; n; n = n->next)
        if (rd(n->item, 8) == id) return rd(n->item, 0x30) == 1;
    return 0;
}

// 1.13c D2Client+0x88350  (ecx = src vec3, edx = dst); lo/hi = floats at 0x6fb87574 / 0x6fb87548.
// Returns 0 on the original's fatal-error path (null arg).
EMSCRIPTEN_KEEPALIVE i32 d2c_88350(const float* src, uint8_t* dst, float lo, float hi) {
    if (!dst || !src) return 0;
    memcpy(dst + 0xc, src, 12);
    float x = src[0], a = lo;
    if (lo < x) { a = x; if (hi <= x) a = hi; }
    float y = src[1], b = lo;
    if (y > lo) b = (hi <= y) ? hi : y;
    float r = a * a + b * b;
    memcpy(dst + 0x18, &r, 4);
    return 1;
}
}
