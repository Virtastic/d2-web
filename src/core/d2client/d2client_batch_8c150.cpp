// src/core/d2client/d2client_batch_8c150.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Globals are parameters; ordinals and unrecovered
// callees are host callbacks. GUESSED ARGUMENTS (decompile shows in_/unaff_ registers): 8C150 (EAX=y, EDX=x),
// 8C1E0 (ESI=dst), 8C210 (EAX=idx), 8C2B0 (ECX=unit, EAX=obj), 8C370 (EAX=obj), 8C5B0 (EDI=mode, EDX=pt),
// 8C600 (EAX=w, ECX=h, ESI=cell, EDX=x0, stack=y0, colour idx), 8C720 (ESI=str), 8C760 (EDI=out, arg=unit),
// ordinal calls with hidden register arguments take none.
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline int32_t ri32(uint32_t a) { return *(const int32_t*)(uintptr_t)a; }

// 1.13c D2Client+0x8C150 (77 B): is (x,y) inside either of two 0x18-high strips.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8c150(int32_t y, int32_t x, int32_t a_bd04, int32_t b_bd08, int32_t c_bd0c,
                         int32_t d_bcf0, int32_t e_bcf4, int32_t f_bcf8) {
    if (a_bd04 <= x && x <= b_bd08 && c_bd0c - 0x18 <= y && y < c_bd0c) return 1;
    if (d_bcf0 <= x && x <= e_bcf4 && f_bcf8 - 0x18 <= y && y < f_bcf8) return 1;
    return 0;
}

// 1.13c D2Client+0x8C1E0 (45 B): strcat(dst, ord10003()) twice; second string lands first.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8c1e0(uint16_t* dst, uint16_t* (*ord10003)(void), void (*strcat_w)(uint16_t*, const uint16_t*)) {
    uint16_t* a = ord10003();
    uint16_t* b = ord10003();
    strcat_w(dst, b);
    strcat_w(dst, a);
}

// 1.13c D2Client+0x8C210 (121 B): string-table lookup by idx 0..6; otherwise error path and _exit(-1).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8c210(uint32_t idx, uint32_t (*ord10003)(void), uint32_t (*ord10265)(int),
                         void (*ord10024)(uint32_t, uint32_t), void (*exit_)(int)) {
    if (idx < 7) return ord10003();
    ord10024(0, ord10265(0x69c));  // arg0 = &DAT_6fb8359f (message), host supplies
    exit_(-1);
    return 0;
}

// 1.13c D2Client+0x8C2B0 (183 B): chain of ordinal predicates, final compare against obj+0x60 value.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8c2b0(uint32_t unit, uint32_t obj, int32_t state_bc34, uint32_t obj_60,
                         int (*ord10439)(uint32_t), int (*ord10202)(void), int (*ord10757)(void),
                         char (*ord11151)(void), uint32_t (*ord10366)(void), uint32_t (*ord10508)(uint32_t)) {
    if (!unit || !obj || state_bc34 == 10 || state_bc34 == 0xb) return 0;
    if (!ord10439(unit) || !ord10202() || !ord10757() || !ord11151()) return 0;
    if (!ord10202() || ord10202()) return 0;
    if (!obj_60) return 0;
    uint32_t a = ord10366();
    return ord10508(obj_60) < (a & 0xff);
}

// 1.13c D2Client+0x8C370 (108 B): class tag at obj+0x80 selects a skill-check.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8c370(uint32_t tag_80, uint32_t arg_973b, int (*ord10174)(uint32_t, int, int)) {
    int sk;
    if (tag_80 == 0x20737361) sk = 9;       // "ass "
    else if (tag_80 == 0x207a7978) sk = 0x14;  // "xyz "
    else if (tag_80 == 0x20327274) {        // "tr2 "
        if (ord10174(arg_973b, 0x25, 8) == 0) return 1;
        return ord10174(arg_973b, 0x25, 7) != 0;
    } else return 0;
    return ord10174(arg_973b, sk, 5) == 0;
}

// 1.13c D2Client+0x8C5B0 (65 B): bounds-checked call of 68970(pt[0]) unless mode==2.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8c5b0(int32_t mode, const int32_t* pt, int32_t w_9e14, int32_t h_9e18, void (*f68970)(int32_t)) {
    if (mode == 2) return;
    if (pt[0] > -1 && pt[0] < w_9e14 && pt[2] > -1 && pt[2] < h_9e18) f68970(pt[0]);
}

// 1.13c D2Client+0x8C600 (166 B): fill a w*h block of cells with colour index `ci`.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8c600(uint32_t w, uint32_t h, int32_t x0, int32_t y0, int32_t ci, uint32_t cell,
                     int32_t w_9e14, int32_t h_9e18, const uint8_t* cols_b330,
                     void (*ord10014)(int, int, int, int, int, int)) {
    int32_t xend = (int32_t)(w & 0xff) + x0;
    for (int32_t x = x0; x < xend; x++)
        for (int32_t y = y0; y < (int32_t)((h & 0xff) + y0); y++) {
            int32_t px = (int32_t)(*(const uint8_t*)(uintptr_t)(cell + 0x14)) * y + ri32(cell + 4);
            int32_t py = (int32_t)(*(const uint8_t*)(uintptr_t)(cell + 0x15)) * x + ri32(cell + 0xc);
            if (px > -1 && px < w_9e14 && py > -1 && py < h_9e18)
                ord10014(px, py, *(const uint8_t*)(uintptr_t)(cell + 0x14) + px,
                         *(const uint8_t*)(uintptr_t)(cell + 0x15) + py, cols_b330[ci], 0);
    }
}

// 1.13c D2Client+0x8C6B0 (99 B): fill the five-colour table at 0x6FBBB330.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8c6b0(uint8_t* cols_b330, uint8_t (*ord10190)(int, int, int)) {
    cols_b330[0] = ord10190(0x80, 0, 0);
    cols_b330[1] = ord10190(0, 0x80, 0);
    cols_b330[2] = ord10190(0, 0, 0x80);
    cols_b330[3] = ord10190(0x80, 0x80, 0);
    cols_b330[4] = ord10190(0x80, 0x40, 0x40);
}

// 1.13c D2Client+0x8C720 (55 B): index of the last ']' (wide) at idx>0, scanning down from strlen.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_8c720(const uint16_t* s, int32_t (*strlen_w)(const uint16_t*)) {
    int32_t i = strlen_w(s);
    while (i > 0 && s[i] != 0x5d) i--;
    return i;
}

// 1.13c D2Client+0x8C760 (329 B): sets *out and returns eligibility from a ladder of ordinal predicates.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_8c760(uint32_t unit, uint32_t* out, uint32_t flag_9854,
                        int (*ord10202)(void), int (*ord10744)(void), int (*ord10326)(void),
                        int (*ord10244)(void), int32_t (*f216d0)(int32_t)) {
    *out = 0;
    if (!flag_9854) return 0;
    int32_t r = 0;
    if (!ord10202() || ord10202()) return 0;
    if (ord10744() || ord10744()) { r = 1; *out = 1; }
    int32_t id = f216d0(unit ? ri32(unit + 0xc) : -1);
    bool set = false;  // reach LAB_8c8a0 (force out=1,r=1)
    if (id < 0x168) {
        if (id == 0x167) {
            if (ord10744()) { r = 1; *out = 1; }
            if (ord10744() && !ord10326()) set = true;
        } else if (id == 0x10f) {
            if (!ord10744()) { /* falls to 83f */ } else set = true;
        } else if (id == 0x152) {
            if (ord10744()) set = true;
            else if (!ord10744()) { /* 83f */ } else set = true;
        }
    } else {
        if (id == 0x230) {
            if (ord10744() && !ord10326()) { r = 1; *out = 1; }
            if (ord10744()) set = true;
        } else if (id == 0x231) {
            if (ord10744()) { r = 1; *out = 1; }
            if (ord10744()) set = true;
        }
    }
    if (set) { r = 1; *out = 1; }
    if (!r) return 0;
    return ord10244() ? r : 0;
}

// 1.13c D2Client+0x8C8B0 (361 B): draw the 12 text rows (stride 0xe table at 0x6FB8139C), splitting at '\n'.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8c8b0(const int32_t* tab_139c, int32_t x_b9a0, int32_t y_b9a4,
                     void (*ord10184)(void), const uint16_t* (*ord10003)(void), void (*ord10028)(void),
                     void (*ord10150)(int, int, int)) {
    ord10184();
    for (uint32_t off = 0; off <= 0xa7; off += 0xe) {
        uint16_t buf[100] = {0};
        const uint16_t* src = ord10003();
        for (int i = 0; i < 99 && src[i]; i++) buf[i] = src[i];
        buf[99] = 0;
        const int32_t* e = (const int32_t*)((const uint8_t*)tab_139c + off);
        int32_t y = e[1] - y_b9a4, x = e[0] + x_b9a0;
        int32_t len = 0; while (buf[len]) len++;
        bool split = false;
        for (int32_t i = 0; buf[i]; i++)
            if (buf[i] == 10) { buf[i] = 0; if (i < len) split = true; break; }
        if (split) { ord10028(); ord10150(y - 4, 0, 0); ord10028(); y += 4; }
        else if (x) ord10028();
        ord10150(y, 0, 0);
    }
    ord10184();
}

// 1.13c D2Client+0x8CA20 (83 B): reset UI state globals; unless ord11017(bbfc->+0x60) is true, reset 4 more.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_8ca20(int32_t* g_beec, int32_t* g_bc34, int32_t* g_b2a4, int32_t* g_b430, uint32_t bbfc_60,
                     uint32_t has_bbfc, int32_t* g_b864, int32_t* g_b858, int32_t* g_b854, int32_t* g_b834,
                     int (*ord11017)(uint32_t)) {
    *g_beec = 0; *g_bc34 = 0; *g_b2a4 = 0; *g_b430 = 0;
    if (has_bbfc && ord11017(bbfc_60)) return;
    *g_b864 = 1; *g_b858 = 0; *g_b854 = 5; *g_b834 = 0;
}
