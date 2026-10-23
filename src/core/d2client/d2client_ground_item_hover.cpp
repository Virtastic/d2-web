// src/core/d2client/d2client_ground_item_hover.cpp -- 1.13c D2Client+0xA68E0 (910 bytes), the
// unit hover/click validator, from build-wasm/ghidra-d2client/decomp/0xA68E0.c. Mapping lane: every
// import ordinal and unrecovered callee is a host seam in D2ClientHoverEnv; the unit is read as
// 32-bit byte offsets.
#include <cstdint>
#include <emscripten.h>

struct D2ClientHoverEnv {
    uint32_t (*rd32)(uintptr_t addr);
    // Globals: DAT_6fbcc414 (view mode), DAT_6fb8bc48 (viewport h), DAT_6fbcb828 (cursor y).
    uint32_t view_mode, viewport_h, cursor_y;
    uint32_t cur_unit;                                   // DAT_6fbcbbfc
    uint32_t (*a62a0)(uint32_t cur);                     // 0xA62A0
    uint32_t (*a51f0)();                                 // 0xA51F0
    uint32_t (*a5b90)();                                 // 0xA5B90
    uint32_t (*a6070)();                                 // 0xA6070
    uint32_t (*a5440)();                                 // 0xA5440
    uint32_t (*o10175)();                                // Ordinal_10175 (switch selector, 0..2)
    void     (*o10350)(uintptr_t unit, int32_t* out2);   // Ordinal_10350 -> iStack_28/24
    uint32_t (*o10651)(uintptr_t unit);                  // unit screen x
    uint32_t (*o11142)(uintptr_t unit);                  // unit screen y
    uint32_t (*f1a550)(uint32_t x, uint32_t y, int32_t z);   // 0x6A550 (case 0)
    uint32_t (*f1260)();                                 // 0x1260 (case 1), returns ptr; +0xec byte
    uint8_t  (*rd8)(uintptr_t addr);
    void     (*o10569)(uintptr_t unit, int32_t* a, int32_t* b);
    int32_t  (*a6730)(uintptr_t unit, int32_t* p9, int32_t* out40);  // 0xA6730 rect builder
    uint8_t  (*e670)();                                  // 0x6E670
    void     (*fatal)(uint32_t line);                    // Fog assert + _exit(-1), no return
};

// 1.13c D2Client+0xA68E0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a68e0(uintptr_t unit, uint32_t param_2, uint32_t param_3, int32_t hit_test,
                         const D2ClientHoverEnv* e) {
    (void)param_2; (void)param_3;
    if (e->view_mode == 1) { if (e->viewport_h / 2 <= e->cursor_y) return 0; }
    else if (e->view_mode == 2) { if (e->cursor_y < e->viewport_h / 2) return 0; }
    else if (e->view_mode == 3) return 0;
    if (unit && ((e->rd32(unit + 0xC4) >> 0x15) & 1)) return 0;
    if (!e->a62a0(e->cur_unit) && !e->a51f0() && !e->a5b90()) return 0;
    bool ok = false;
    if (!e->a6070()) {
        if (!unit) return 0;
        ok = (e->rd32(unit + 0xC4) >> 1) & 1;
    } else {
        if (!unit) return 0;
        if ((e->rd32(unit + 0xC4) >> 1) & 1) ok = true;
        else if (e->rd32(unit) == 1 && e->a5440()) ok = (e->rd32(unit + 0xC4) >> 3) & 1; // UNVERIFIED: decompile tests (EDX>>2)&1, EDX a register leftover from 0xA5440
    }
    if (!((e->rd32(unit + 0xC8) >> 7) & 1)) return 0;
    if (!ok) return 0;
    if (!hit_test) return 1;
    uint32_t sel = e->o10175();
    int32_t r[2] = {0, 0};
    e->o10350(unit, r);
    uint32_t sx = e->o10651(unit), sy = e->o11142(unit);
    if (sel == 0) { if (!e->f1a550(sx, sy, 0)) return 0; }
    bool wide = false;
    if (sel != 0) {
        if (sel == 2) e->fatal(0x741);
        if (sel == 1) { uint32_t p = e->f1260(); wide = p && e->rd8(p + 0xec) >= 2; }
    } else wide = true;
    int32_t a = 0, b = 0, o40 = 0;
    e->o10569(unit, &a, &b);
    if (e->rd32(unit) != 0 || e->rd32(unit + 4) != 4 || e->rd32(unit + 0x10) != 0x12) {
        if (wide) {
            for (uint32_t n = 0;; ) {
                if (e->a6730(unit, r, &o40) && e->e670() && !(r[0] < o40 || r[0] > 0 || r[1] < 0) && !(0 < r[1])) break;
                if (++n > 0xf) return 0;
            }
        } else if (!e->a6730(unit, r, &o40) || r[0] < o40 || r[0] > 0 || r[1] < 0 || 0 < r[1]) return 0;
    }
    return 1;
}
