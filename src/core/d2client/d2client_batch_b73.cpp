// src/core/d2client/d2client_batch_b73.cpp -- four 1.13c D2Client.dll rows (alpha lane), from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000. Globals and register args are
// parameters; unrecovered callees are host callbacks.
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

// 1.13c D2Client+0x87F30 (173 B): out = {dx, 2*dy, 640}. dx/dy are the already-differenced
// results of the two Ordinal_10628 / 0xCACC4 reads (retail subtracts in registers).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_87f30(float* out, int32_t dx, int32_t dy) {
    out[2] = 640.0f;
    out[0] = (float)dx;
    out[1] = (float)dy + (float)dy;
}

// 1.13c D2Client+0x88440 (132 B): picks between two float constants ([0x6FB87568] / [0x6FB87578]).
// ecx_idx = EAX index; tab_24c/cnt_250 = [0x11C24C]/[0x11C250] (0x8E-byte rows); tab_254/cnt_258 =
// [0x11C254]/[0x11C258] (0x58-byte rows); callbacks: ord10839(local, p, 2), sub_5EB90() -> index.
// Row bytes are read through `rd`. Returns 0 -> const_578, 1 -> const_568.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_88440(int32_t idx, uint32_t tab_24c, int32_t cnt_250, uint32_t tab_254, int32_t cnt_258,
                         uint32_t local_bbfc, uint32_t param_1,
                         uint32_t (*rd)(uint32_t addr, uint32_t size),
                         uint32_t (*ord10839)(uint32_t, uint32_t, uint32_t),
                         uint32_t (*sub_5EB90)(void)) {
    uint32_t row = 0;
    if (tab_24c && idx >= 0 && idx < cnt_250)
        row = (uint32_t)idx * 0x8E + tab_24c;
    bool use568;
    if (row == 0) {
        use568 = ord10839(local_bbfc, param_1, 2) == 0;
    } else {
        if (rd(row + 0x47, 4) == 4) return 0;
        if (rd(row + 0x60, 4) != 0xCA) {
            use568 = ord10839(local_bbfc, param_1, 2) == 0;
        } else {
            uint32_t i = sub_5EB90() & 0xFF;
            uint32_t r2 = (tab_254 && i < (uint32_t)cnt_258) ? i * 0x58 + tab_254 : 0;
            if (r2 == 0) return 0;
            use568 = (rd(r2 + 0x18, 1) & 0xFF) == 0;
        }
    }
    return use568 ? 1u : 0u;
}

// 1.13c D2Client+0x8A4D0 (105 B): set rect1 = {l,t,r,b}; rect2 = {-0x50,-0x50,w+0x50,h-0x2f};
// widened (top-0x1e, bottom+0x1e, left-0x2d) when `wide`; flags |= 1. s = uint32_t[9]: flags, rect1[4], rect2[4].
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8a4d0(uint32_t* s, int32_t l, int32_t t, int32_t r, int32_t b,
                     int32_t scr_w, int32_t scr_h, uint32_t wide) {
    if (!s) return;
    int32_t* r1 = (int32_t*)(s + 1);
    int32_t* r2 = (int32_t*)(s + 5);
    r1[0] = l; r1[1] = t; r1[2] = r; r1[3] = b;
    r2[0] = -0x50; r2[1] = -0x50; r2[2] = scr_w + 0x50; r2[3] = scr_h - 0x2f;
    if (wide) { r2[0] -= 0x2d; r2[1] -= 0x1e; r2[3] += 0x1e; }
    s[0] |= 1;
}

// 1.13c D2Client+0x88A70: not implemented (register-passed args, unrecovered callees); row skipped.
