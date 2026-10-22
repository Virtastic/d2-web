// src/core/d2client/d2client_batch_619e0.cpp -- 1.13c D2Client.dll sub_619E0 (alpha lane), from
// build-wasm/ghidra-d2client/decomp/0x619E0.c. Base 0x6FAB0000. Globals are parameters; unrecovered
// callees and ordinals are host callbacks. HIDDEN-REGISTER: EAX=unit, ESI=color, ECX=pass-through (GUESSES).
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

struct Am619E0 {  // callbacks + globals, retail argument order
    int (*ord10494)(void); int (*sub_61490)(int); int32_t (*ord10651)(void); int32_t (*ord11142)(void);
    void (*sub_5f1c0)(int, uint32_t); void (*ord10920)(void); void (*sub_601a0)(uint32_t, int, uint8_t);
    uint32_t (*sub_1760)(int, int); void (*ord10184)(void); void (*sub_55d90)(void);
    void (*ord10028)(int, int, int); void (*ord10150)(void); void (*ord10003)(void);
    void (*ord10350)(const char*); int (*ord10866)(void); int* (*sub_560d0)(int); int16_t (*sub_55b60)(int);
    int32_t cell_div_1a16b0, org_x_c1f8, org_y_c1fc, b16b8, b16bc; uint8_t col_c213;
    const int32_t* bounds_c228_c234; uint32_t local_unit_bbfc;
};

// 1.13c D2Client+0x619E0 (786 B) NAMED: unit in EAX, color in ESI (guess).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_619e0(uint32_t unit, uint32_t esi, int ecx, const Am619E0* g) {
    int32_t kind = unit ? (int32_t)rd32(unit) : 6;
    if (kind == 0) {  // player: unless mode is neither 0 nor 0x11 (and flag clear), gate on ord10494
        bool gate = true;
        if (!(rd32(unit + 0xc4) & 0x10000)) { int32_t m = (int32_t)rd32(unit + 0x10); gate = (m == 0 || m == 0x11); }
        if (gate && g->ord10494() == 0) return;
    }
    if (!g->sub_61490(ecx)) return;
    int32_t px = g->ord10651(), py = g->ord11142();
    int32_t x = (px / g->cell_div_1a16b0 - g->org_x_c1f8) + 8;
    int32_t ty = py / g->cell_div_1a16b0 - g->org_y_c1fc, y = ty - 8;
    const int32_t* b = g->bounds_c228_c234;  // [c228,c22c,c230,c234]
    if (!(b[0] <= x && x <= b[2] && b[1] <= y && y <= b[3])) return;
    if (kind == 0) {
        if (g->b16b8 != 0 || (uint8_t)esi != g->col_c213) g->sub_5f1c0(x, esi);
        if (g->b16bc && g->b16b8 && unit != g->local_unit_bbfc) { g->ord10920(); g->sub_601a0(unit, x, (uint8_t)y); }
    } else if (kind == 1) {
        g->sub_5f1c0(x, esi);
        if (g->b16bc) {
            if (g->sub_1760(ecx, 9)) { g->ord10184(); g->sub_55d90(); g->ord10028(ty - 0x12, 4, 0); g->ord10150(); g->ord10184(); }
            if (g->b16bc && g->b16b8) {
                g->ord10350("..\\Source\\D2Client\\UI\\automap.cpp");  // retail tests its (stack) result == 0
                int* p;
                if (g->ord10866() && (p = g->sub_560d0(ecx)) && *p == 0) {
                    int16_t a = g->sub_55b60(ecx);
                    if (a == -1 || a != g->sub_55b60(ecx)) { g->ord10920(); g->sub_601a0((uint32_t)(uintptr_t)p, x, (uint8_t)y); }
                }
            }
        }
    } else if (kind == 2) {
        if (g->b16bc && unit && rd32(unit + 4) == 0x10b) {
            g->ord10184(); g->ord10003(); g->ord10028(ty - 0x12, esi & 0xff, 0); g->ord10150(); g->ord10184();
            return;
        }
        g->sub_5f1c0(x, esi);
    }
}
