// src/core/d2client/light_fill_a9100.cpp -- light-record grid fill.
// 1.13c D2Client+0xA9100 (887 B)
// Mapping lane: globals/callees are host seams. ESI = light record (+0x14 y, +0x18 radius,
// +0x2C done flag, +0x30 out buffer). grid = DAT_6fbacde8 (int32[0x400], stride 0x41 per row of the
// 0x40-wide window), occl = DAT_6fbb0de8 (int32[0x40*0x40]).
// UNVERIFIED: iStack_c (decompiler leaves it unset; taken as `limit`); the EAX/ECX halves of the
// a8310 register pairs (only the visible arg is passed); Fog ordinals 10331/10265/10024/10482.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t R32(uintptr_t p) { return *(const uint32_t*)p; }
static inline void W32(uintptr_t p, uint32_t v) { *(uint32_t*)p = v; }

struct D2ClientA9100Env {
    uintptr_t (*get_unit)();                          // FUN_6fb58d30
    uintptr_t (*ord10331)(uintptr_t unit);            // unit -> room (0 => fatal 0x414)
    int32_t (*unit_x)(uintptr_t unit);                // 0x1630
    int32_t (*unit_y)(uintptr_t unit);                // 0x1660
    int16_t (*ord10482)(uintptr_t room, int32_t x, int32_t y, int32_t mask);
    void (*a8470)(uintptr_t rec, int32_t x, int32_t y);
    void (*a8310)(int32_t v);
    void (*fatal)(uint32_t msg);                      // Ordinal_10265 + 10024 + _exit(-1)
    int32_t limit;                                    // iStack_c (unverified)
    int32_t* grid;
    uint32_t* occl;
};

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a9100(uintptr_t rec, const D2ClientA9100Env* e) {
    int32_t r = (int32_t)R32(rec + 0x18);
    if (!(0 < r && r < 0x100)) return;
    for (int i = 0; i < 0x400; i++) e->grid[i] = 0;
    int32_t cy8 = r >> 3, cx8 = (int32_t)R32(rec + 0x14) >> 3;
    uintptr_t u = e->get_unit();
    if (!u) return;
    uintptr_t room = e->ord10331(u);
    if (!room) e->fatal(0x414);
    int32_t ux = e->unit_x(u), uy = e->unit_y(u);
    for (int32_t j = 0; j < 0x40; j++)
        for (int32_t i = 0; i < 0x40; i++)
            e->occl[j * 0x40 + i] = e->ord10482(room, i + ux - 0x20, j + uy - 0x20, 0x22) ? 0x10 : 0;
    if (e->limit > 1) {
        int32_t b24 = cy8 * 8 + 0x14, b28 = cx8 * 8 + 0x14, b1c = cy8 * 8 - 0xc, b20 = cx8 * 8 - 0xc;
        int32_t v6 = 0x1e, v4 = 0x22;
        do {
            if (v4 - 0x20 >= 0) {
                int32_t b34 = cy8 * 8 + 4, b38 = cx8 * 8 + 4, v13 = 0x20, v14 = 0x20;
                int32_t b30 = b38, b2c = b34, n = v4 - 0x1f;
                do {
                    e->a8470(rec, b30, b1c);  e->a8310(v14);
                    e->a8470(rec, b38, b1c);  e->a8310(v13);
                    e->a8470(rec, b30, b24);  e->a8310(v14);
                    e->a8470(rec, b38, b24);  e->a8310(v13);
                    e->a8470(rec, b28, b2c);  e->a8310(v4);
                    e->a8470(rec, b28, b34);  e->a8310(v4);
                    e->a8470(rec, b20, b2c);  e->a8310(v6);
                    e->a8470(rec, b20, b34);  e->a8310(v6);
                    b38 += 8; b34 += 8; b30 -= 8; b2c -= 8; v14--; v13++;
                } while (--n != 0);
            }
            b28 += 8; b24 += 8; b20 -= 8; b1c -= 8; v6--;
        } while (v4++ - 0x1f <= e->limit);
    }
    if (R32(rec + 0x2C) != 0) e->fatal(0x454);
    int32_t* dst = (int32_t*)(uintptr_t)R32(rec + 0x30);
    if (!dst) e->fatal(0x455);
    if (cy8 >= 0) {
        int32_t* row = e->grid + (0x20 - cy8) * 0x41;
        for (int32_t j = 0; j <= cy8; j++, row += 0x40)
            for (int32_t i = 0; i <= cy8; i++) *dst++ = row[i];
    }
    W32(rec + 0x2C, 1);
}
