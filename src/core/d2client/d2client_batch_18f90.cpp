// src/core/d2client/d2client_batch_18f90.cpp -- five UNMAPPED 1.13c D2Client.dll rows (alpha lane).
// Base 0x6FAB0000. Globals are parameters (the caller reads the live value); unrecovered callees and
// Ordinals are host callbacks. Register-passed implicit arguments are named after their register.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x18F90 (151 B): EAX = unit; EBX = pointer to a word; param_1 = optional out.
// unit != [0xCBBFC] -> 1. EBX null or *EBX == 0 -> 0. m = FUN_6fb22080(); *param_1 = m; o = Ordinal_10858();
// o != 0: when 0<m && (m<3 || m==4) && (mask & *(u8*)(o+7)) [mask = low byte of gdwBitMasks[0x10]]:
// *EBX = Ordinal_10630(); m = FUN_6fb22080(). m==5 or m==0 -> 1; else 0x18F40(unit). Return 0.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_18f90(uint32_t unit, uint32_t local_cbbfc, uint32_t* ebx, int32_t* param_1,
                         int32_t (*f22080)(void), uintptr_t (*ord10858)(void), uint32_t (*ord10630)(void),
                         uint8_t mask, void (*f18f40)(uint32_t)) {
    if (unit != local_cbbfc) return 1;
    if (!ebx || *ebx == 0) return 0;
    int32_t m = f22080();
    if (param_1) *param_1 = m;
    uintptr_t o = ord10858();
    if (o) {
        if (m > 0 && (m < 3 || m == 4) && (mask & *(const uint8_t*)(o + 7))) {
            *ebx = ord10630();
            m = f22080();
        }
        if (m == 5 || m == 0) return 1;
        f18f40(unit);
    }
    return 0;
}

// 1.13c D2Client+0x1BC0 (75 B): EDI = unit, ESI = optional out flag. unit && (*(u32*)(unit+200)>>3 & 1):
// Ordinal_10866() != 0 -> p = FUN_6fb560d0(ECX); p != null -> *ESI = 1, return p[0x18].
// Else *ESI = 0, return *(u32*)(unit+0x60).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_1bc0(uintptr_t unit, uint32_t* esi, uint32_t ecx,
                       int32_t (*ord10866)(void), int32_t* (*f560d0)(uint32_t)) {
    if (unit && ((*(const uint32_t*)(unit + 200) >> 3) & 1)) {
        if (ord10866() != 0) {
            int32_t* p = f560d0(ecx);
            if (p) {
                if (esi) *esi = 1;
                return p[0x18];
            }
        }
    }
    if (esi) *esi = 0;
    return *(const int32_t*)(unit + 0x60);
}

// 1.13c D2Client+0x10B60 (222 B): Ordinal_476(); run the 1 init fn at table 0xD3134 (any 0 -> 0);
// [0xF4894] = 1; call each non-null of the 0x34 entries at 0xF4898 (retail loop is 0xD0 bytes = 52 slots);
// [0xF4894] == 0 -> 0. Then Ordinal_422("Diablo II") != 0 -> Ordinal_475(...) reads the command line;
// when the user-name buffer [0x119928] is empty, GetUserNameA else "noname". Return 1.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_10b60(uint32_t (*const* init_tbl)(void), uint32_t n_init,
                         void (*const* hooks)(void), uint32_t* flag_f4894,
                         void (*ord476)(void), int32_t (*ord422)(const char*), void (*ord475)(void),
                         char* user_119928, void (*get_user_name)(char*, uint32_t)) {
    ord476();
    for (uint32_t i = 0; i < n_init; i++)
        if (init_tbl[i]() == 0) return 0;
    *flag_f4894 = 1;
    for (uint32_t i = 0; i < 0xd0 / 4; i++)
        if (hooks[i]) hooks[i]();
    if (*flag_f4894 == 0) return 0;
    if (ord422("Diablo II") != 0) ord475();
    if (user_119928[0] == 0) get_user_name(user_119928, 0xf);   // wsprintfA("%s","noname") on failure
    return 1;
}

// 1.13c D2Client+0x13FD0 (113 B): FUN_6fac3f40 three times.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_13fd0(void (*f13f40)(void)) { f13f40(); f13f40(); f13f40(); }

// 1.13c D2Client+0x10C80 (334 B): main client loop. [0x11B980] = EAX; Ordinal_10144(); Ordinal_10145(0,1);
// [0x11C558] = hInstance; GetModuleFileNameA -> 0x11B868; 0x10B60 and FUN_6fb12cf0 must be non-zero;
// Ordinal_10100/10143/10227->10016/10008 (0 -> return 0); 10004; 10016(0, cfg[0x15]==6)->10943.
// state [0xF8C5C] = 3, [0xF8C9C] = 1; loop state = fn[state](table 0xF8C60); state>6 -> error 0x298,
// state<1 -> 0x299 (Ordinal_10265 -> Ordinal_10024 -> _exit(-1)); state==1: teardown
// (10144,11023,10005,12c90,10300,5fc10); [0xF8C9C] != 0 -> [0x11C55C]=0, return 0; else cfg[0x15]
// in {2,3,6,7} -> 3, otherwise 4 (stored in [0x11C55C]). Host callback owns the prologue and teardown.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_10c80(uint32_t cfg_type_x15, uint32_t* state_f8c5c, uint32_t* flag_f8c9c,
                         uint32_t* out_11c55c, uint32_t (*const* states)(void),
                         uint32_t (*prologue)(void), void (*teardown)(void), void (*fatal)(uint32_t)) {
    if (!prologue()) return 0;
    *state_f8c5c = 3;
    *flag_f8c9c = 1;
    for (;;) {
        *state_f8c5c = states[*state_f8c5c]();
        if (*state_f8c5c > 6) { fatal(0x298); return 0; }
        if (*state_f8c5c < 1) { fatal(0x299); return 0; }
        if (*state_f8c5c == 1) {
            teardown();
            if (*flag_f8c9c != 0) { *out_11c55c = 0; return 0; }
            switch (cfg_type_x15) {
            case 2: case 3: case 6: case 7: *out_11c55c = 3; return 3;
            default: *out_11c55c = 4; return 4;
            }
        }
    }
}
