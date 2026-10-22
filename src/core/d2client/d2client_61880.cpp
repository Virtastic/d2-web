// 1.13c D2Client+0x61880 (342 B): per-record cell-node emit. Record in ESI, view ctx in EDI, one stack arg
// (passed to 0x61320). Callees and ordinals are host callbacks after the retail arguments.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
static inline void wr16(uint32_t a, int16_t v) { *(int16_t*)(uintptr_t)a = v; }

// 1.13c D2Client+0x61880
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_61880(uint32_t rec_esi, uint32_t ctx_edi, uint32_t arg,
                     uint32_t (*cc46)(uint32_t), uint32_t (*c12a)(uint32_t), uint32_t (*c130)(uint32_t),
                     uint32_t (*c136)(uint32_t), int32_t (*cc40)(uint32_t, uint32_t, uint32_t, uint32_t),
                     uint32_t (*sub_5f6b0)(void), void (*cc58)(int32_t*, int32_t*),
                     void (*sub_61320)(uint32_t, uint32_t), uint32_t (*bf92)(uint32_t),
                     void (*bf80)(uint32_t, uint32_t), uint32_t str_83359f, void (*exit3664)(int32_t)) {
    uint32_t f = rd32(rec_esi + 0x14);
    if (f & 0x40000) return;
    wr32(rec_esi + 0x14, f | 0x40000);
    uint32_t a = cc46(rd32(rd32(ctx_edi + 0x58) + 0x1d0));
    uint32_t b = c12a(rd32(rec_esi + 0x18));
    uint32_t c = c130(rd32(rec_esi + 0x18));
    uint32_t d = c136(rd32(rec_esi + 0x18));
    int32_t id = cc40(a, b, c, d);
    if (id == -1) return;
    uint32_t node = sub_5f6b0();
    int32_t x = (int32_t)(rd32(ctx_edi + 0x34) + rd32(rec_esi + 8));
    int32_t y = (int32_t)(rd32(ctx_edi + 0x38) + rd32(rec_esi + 0xc));
    cc58(&x, &y);
    x /= 10; y /= 10;
    uint32_t code;
    if (x < -0x8000 || x > 0x7fff) code = 0x391;
    else if (y < -0x8000 || y > 0x7fff) code = 0x392;
    else if (id < -0x8000 || id > 0x7fff) code = 0x393;
    else {
        if ((int32_t)rd32(rec_esi + 0x1c) >= 0x10) y += 0x18;
        wr16(node + 4, (int16_t)id); wr16(node + 6, (int16_t)x); wr16(node + 8, (int16_t)y);
        sub_61320(node, arg);
        return;
    }
    bf80(str_83359f, bf92(code));
    exit3664(-1);
}
