// src/core/d2client/item_tooltip_c0850.cpp -- 1.13c D2Client+0xC0850 (1167 B), the hover-item tooltip builder.
// Mapping lane: globals are parameters; ordinals (unverified, via thunks) and unrecovered callees are host
// callbacks. Ordinal numbers are the decompiler's import names, NOT verified. Stack-register args guessed.
#include <cstdint>
#include <emscripten.h>

typedef uint16_t wch;
struct C0850Env {
    void (*ord10184)();                                  // enter/leave (called first and last)
    const wch* (*ord10003)(uint32_t id);                 // string table lookup (ids not shown by decompiler)
    void (*ustrcpy)(wch*, const wch*);
    void (*ustrcat)(wch*, const wch*);
    void (*win2uni)(wch*, const char*, int);
    const wch* (*fn_55d90)();                            // FUN_6fb55d90
    void* (*fn_1260)();                                  // FUN_6fab1260 -> struct* (byte +0xd)
    void* (*fn_1230)();                                  // FUN_6fab1230 -> struct* (ushort +8)
    uint32_t (*ord10331)(const int32_t* unit);
    int (*ord10057)(uint32_t);
    int (*ord10009)();
    int (*fn_70590)();                                   // FUN_6fb70590
    int (*fn_6ea10)(int32_t out[4]);                     // FUN_6fb6ea10, bool
    void (*ord10028)();
    void (*ord10070)(int, uint32_t, int, int);
    int (*ord10255)(const int32_t* unit);
    int (*ord10239)(const int32_t* unit);
    int (*fn_701e0)(const int32_t* unit);                // FUN_6fb701e0
    void (*fn_6e6d0)(uint32_t ecx, const wch* s, uint32_t mode);
    void (*fn_6fca0)(const int32_t* unit);
    void (*ord10085)(int, int, int);
};

// 1.13c D2Client+0xC0850
// unit = param_1 (int[]: [0]=type,[1]=classId,[4]=mode,[5]=->data with flags byte +0x16).
// tbl_row = items-table row for classId (stride 0x1a8; bytes +0xd flags, short +6), NULL if out of range;
// mask4/mask1c = gdwBitMasks[1]/[7]. g_58bc58 = DAT 0xDBC58 (in/out), out_cc780/g_c9721/g_c3a68 = DAT 0xCC780/0xC9721/0xC3A68.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2c_c0850(const int32_t* unit, const uint8_t* tbl_row, uint32_t mask4, uint32_t mask1c, int32_t* g_58bc58,
               int32_t* out_cc780, uint8_t g_c9721, int32_t g_c3a68, const C0850Env* E) {
    wch buf[0x1000] = {0}, buf2[8] = {0}, line[0x800] = {0};
    int32_t cls = unit ? unit[1] : -1, v[4];
    E->ord10184();
    if (tbl_row && (mask4 & tbl_row[0xd]) == 0 && (mask1c & tbl_row[0xd]) != 0 && *(const int16_t*)(tbl_row + 6) != 0) {
        const uint8_t* s = (const uint8_t*)E->fn_1260();
        if (s) *g_58bc58 -= s[0xd];
        E->ustrcpy(buf, E->ord10003(0)); E->ustrcat(buf, E->ord10003(0));
        E->ustrcat(buf, E->fn_55d90()); E->ustrcat(buf, E->ord10003(0));
        if (unit && unit[4] == 0xc) {
            if (E->ord10057(E->ord10331(unit))) { E->ord10184(); return; }  // retail returns without the trailing call
            const wch* p;
            if (E->ord10009()) {
                wch t[2]; E->win2uni(t, "", 2); E->ustrcat(buf2, t);
                E->ustrcat(buf2, E->ord10003(0)); E->win2uni(t, "", 2); p = t;
                E->ustrcat(buf2, p);
            } else E->ustrcat(buf2, E->ord10003(0));
            E->ustrcat(buf, E->ord10003(0));
        }
        E->ustrcat(buf, E->ord10003(0));
        int col = E->fn_70590();
        uint32_t m = 0;
        if (unit && unit[0] == 1 && unit[5]) {
            uint8_t f = *(const uint8_t*)(uintptr_t)(unit[5] + 0x16);
            if (f & 8) m = 4;
            if (f & 4) m = 3;
        }
        static const int16_t spec[] = {0xf3, 0x14d, 0xf2, 0xd3, 0x220, 0x23a, 0xe5, 0x2c4, 0x2c3, 0x2c2, 0x2c1, 0x2c5, 0x2c0};
        for (int16_t k : spec) if (cls == k) m = 4;
        if (E->fn_6ea10(v)) {
            E->ord10028(); E->ord10070(0x20, m, 0, col);
            const uint8_t* g = (const uint8_t*)E->fn_1230();
            int16_t sv = 0;
            if (E->ord10255(unit)) sv = 0x275e; else if (E->ord10239(unit)) sv = 0x275d;
            if (E->fn_701e0(unit) == 0) {
                uint16_t gid = *(const uint16_t*)(g + 8);
                if (sv) {
                    E->ustrcpy(line, E->ord10003(sv)); E->ustrcat(line, E->ord10003(0));
                    if (gid != 0x1506) E->ustrcat(line, E->ord10003(0));
                    E->fn_6e6d0(gid, line, m); E->fn_6fca0(unit); E->ord10184(); return;
                }
                if (gid != 0x1506) E->fn_6e6d0(gid, E->ord10003(0), m);
            }
            E->fn_6fca0(unit);
        }
    } else {
        *out_cc780 = 0;
        if (g_c9721 == 0 && g_c3a68 == 0) {
            const uint8_t* s = (const uint8_t*)E->fn_1260();
            if (s) *g_58bc58 -= s[0xd];
            E->fn_55d90(); E->ord10085(*g_58bc58, 0, 1);
        }
    }
    E->ord10184();
}
