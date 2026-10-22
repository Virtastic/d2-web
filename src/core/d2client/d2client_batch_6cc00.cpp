// src/core/d2client/d2client_batch_6cc00.cpp -- 1.13c D2Client+0x6CC00 (1038 B), alpha lane, from
// build-wasm/ghidra-d2client/decomp/0x6CC00.c. Base 0x6FAB0000. Globals are parameters; unrecovered
// callees and ordinals are host callbacks. The decompile reuses stack slots: GUESSes are marked.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

struct H6cc00 {
    uint32_t (*ord10013)(void);
    uint32_t (*ord10175)(void);
    void (*ord10350)(uint32_t, void*, void*, void*);
    uint8_t (*ord10663)(uint32_t);
    void (*ord10569)(uint32_t);
    uint32_t (*ord10164)(void);
    int (*ord11088)(uint32_t);
    uint32_t (*ord10973)(uint32_t, int, int);
    int (*ord10005)(void*, int, int);
    int (*ord10893)(int, uint32_t, void*, int);
    void (*ord10041)(uint32_t, int32_t, int32_t, uint32_t, uint32_t, int32_t);
    bool (*sub_54b90)(void);
    void (*sub_1b700)(void);
    int (*sub_6c490)(uint32_t);
    int (*sub_6e2c0)(uint32_t, void*);
    uint32_t (*sub_01a80)(void);
    uint32_t (*sub_01580)(void);  // returns base of a config struct (+0x18d,+0x1a2)
    void (*sub_1aee0)(uint32_t, uint32_t, int, int, int, int);
};

// 1.13c D2Client+0x6CC00
extern "C" EMSCRIPTEN_KEEPALIVE
int d2_client_6cc00(uint32_t p, uint32_t param2, const H6cc00* h, uint32_t glob_bbfc, int32_t glob_c995c,
                    int32_t glob_c9960, int32_t glob_cc418, int32_t mode_cc414, int32_t half_8bc48,
                    int32_t glob_cc2d8, int32_t glob_cc380, int32_t ebx /*GUESS: unaff_EBX*/) {
    uint32_t mode = h->ord10013();
    if (p && rd32(p) == 5) return 0;
    if (h->sub_54b90()) h->sub_1b700();
    uint32_t typ = p ? rd32(p) : 6, sub = p ? rd32(p + 4) : 0xffffffffu;
    uint32_t t[2] = {typ, sub}, ap = h->ord10175();
    int32_t px = 0, py = 0;  // GUESS: ord10350 outs (stack slots iStack_18/14)
    h->ord10350(p, &param2, t, &ap);
    uint32_t s30 = (p && rd32(p + 0x54)) ? rd32(rd32(p + 0x54) + 0x30) : 0;
    px += s30 ? (int32_t)rd32(s30 + 0x34) : 0;
    py += (s30 ? (int32_t)rd32(s30 + 0x38) : 0) + (s30 ? (int32_t)rd32(s30 + 0x3c) : 0);
    (void)px; (void)py;
    if (p && (int32_t)rd32(p) >= 0 && (int32_t)rd32(p) < 3) return h->sub_6c490(p);

    uint8_t shade = h->ord10663(p);
    h->ord10569(p);
    if (rd32(p + 0x6c) && p != glob_bbfc) h->ord10164();
    uint32_t lvl = shade;  // GUESS: uVar5 seed
    if (h->ord11088(p) == 4) {  // original tests param==4 (register pass-through, unreachable for real units)
        uint32_t v = h->ord10973(p, 0xe, 0);
        lvl = v < 100 ? 0 : v < 500 ? 1 : 3 - (v < 5000);
    }
    uint32_t rec[0x14] = {0};
    rec[16] = lvl;
    if (!h->sub_6e2c0(p, rec) || !h->ord10005(rec, 0, 1)) return 0;
    int32_t x = (glob_cc418 - glob_c9960) + ebx + (int32_t)rec[16];
    int32_t y = ((int32_t)rec[17] + 8) - glob_c995c;
    if (mode_cc414 == 1) x += -((half_8bc48 + ((half_8bc48 >> 31) & 3)) >> 2);
    else if (mode_cc414 == 2) x += (half_8bc48 + ((half_8bc48 >> 31) & 3)) >> 2;
    bool self = h->sub_01a80() == p;
    uint32_t a = shade;
    if (self) { a = (a & 0xff) * 2; if ((shade & 0xff) == 0x20 || a < 0x40) a = 0x40; else if (a > 0xfe) a = 0xff; }
    if (mode != 3) {
        uint32_t col = self ? 7 : 5;
        int32_t pal = (int32_t)rec[18];
        if (mode == 4 && h->ord10893(0, p, &rec[16], 0)) pal = (int32_t)rec[18];  // GUESS: out replaces palette
        h->ord10041(0, x, (int32_t)rec[17], a, col, pal);
        if (rec[13]) h->sub_1aee0(p, a, 0, -1, -1, 0);
        return 1;
    }
    uint32_t c = h->sub_01580();
    uint32_t col = 5;
    if (*(const char*)(uintptr_t)(c + 0x18d) == 1) col = 3;
    if (*(const char*)(uintptr_t)(c + 0x18d) == 2) col = 4;
    if (self) col = 7;
    int32_t pal = (*(const char*)(uintptr_t)(c + 0x1a2) == 0 || glob_cc380 == 0) ? (int32_t)rec[18] : glob_cc2d8;
    h->ord10041(0, x, y, a, col, pal);
    return 1;
}
