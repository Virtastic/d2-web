// src/core/d2win/d2win_batch_b2.cpp -- 1.13c D2Win.dll alpha batch (no spec harness). Retail globals/callees arrive via seams.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Win+0x14DF0 (control SetText; ctl type 1 = textbox)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_14df0(
    int32_t* ctl, const uint16_t* text, int32_t (*ustrlen)(const uint16_t*),
    void (*ustrcpy)(uint16_t*, const uint16_t*), void (*refresh)(), void (*fatal)(uint32_t code)) {
    if (!ctl) { fatal(0x45e); return 0; }
    if (ctl[0] != 1) { fatal(0x460); return 0; }
    int32_t n = ustrlen(text);
    if (n == 0) *(uint16_t*)(ctl + 0x17) = 0; else ustrcpy((uint16_t*)(ctl + 0x17), text);
    ctl[0x97] = n;
    refresh();
    typedef void (*cb_t)(int32_t);
    cb_t cb = (cb_t)(uintptr_t)(uint32_t)ctl[0x9b];
    if (cb) cb(0);
    return 1;
}

// 1.13c D2Win+0x18490 (control free by type; clears *slot). free_by[k] = the per-type destructor
// (0:none 1:159E0 2/3/5/8/10/13:17FC0 4:8B00 6:F8A0 7:16D70 9:116A0 12:A0A0).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_18490(uint32_t* slot, uint32_t (*const* free_by)(void*)) {
    if (!slot || !*slot) return 0;
    int32_t* c = (int32_t*)(uintptr_t)*slot;
    int32_t t = *c;
    if (t < 1 || t > 13 || !free_by[t]) { *slot = 0; return 0; }
    uint32_t r = free_by[t](c);
    *slot = 0;
    return r;
}

// 1.13c D2Win+0x17A90 (set current focus control; frees the previous via 13770)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_17a90(int32_t** cur, int32_t* next, void (*free_ctl)(int32_t*)) {
    if (*cur && next != *cur) free_ctl(*cur);
    *cur = next;
    return 1;
}

// 1.13c D2Win+0x9200 (listbox visible-row count; ctl type 4)
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_9200(int32_t* ctl,
        void (*sel_font)(int32_t), void (*fatal)(uint32_t), int32_t font_h) {
    if (!ctl) { fatal(0x290); return 0; }
    if (ctl[0] != 4) { fatal(0x291); return 0; }
    sel_font(ctl[0x25]);
    int32_t v = font_h + ctl[0x2a];
    return ((ctl[6] - ctl[0x11] * 2) - v) / (ctl[0x29] + v) + 1;
}
