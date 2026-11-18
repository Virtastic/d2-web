// src/core/d2client/unit_sound_bc1a0.cpp -- D2Client.dll 1.13c, base 0x6FAB0000.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

// 1.13c D2Client+0xBC1A0 (100 B, __fastcall ecx = unit; no static callers): looks up record unit+4 in the
// sgptDataTables table (count @+0xB6C, array @+0xB64, stride 0x1A4); if rec i16@+0x2C >= 0 it calls
// Ordinal_11029(unit, rec+0x70, 1) [D2Common, UNVERIFIED ordinal], Ordinal_10240() [UNVERIFIED], then
// 0xBB210(ecx=rec+0x6C, edx=0x109, s={pv, unit, r, unit}). Always returns 1.
// recs = array base, n = count (caller-resolved from sgptDataTables); unit = pointer to unit dword[2] (id at +4).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_bc1a0(uint32_t unit, const uint8_t* unitMem,
                                                         const uint8_t* recs, int32_t n, uint32_t recsAddr,
                                                         D2cHost h) {
    if (!unit) return 1;
    int32_t i = (int32_t)*(const uint32_t*)(unitMem + 4);
    if (i < 0 || i >= n) return 1;
    const uint8_t* rec = recs + (uint32_t)i * 0x1A4;
    if (*(const int16_t*)(rec + 0x2C) < 0) return 1;
    D2cCall a = {}; a.rva = 11029; a.s[0] = unit; a.s[1] = *(const uint32_t*)(rec + 0x70); a.s[2] = 1;
    uint32_t r = h(&a);
    D2cCall b = {}; b.rva = 10240;
    uint32_t pv = h(&b);
    D2cCall c = {}; c.rva = 0xBB210; c.ecx = *(const uint32_t*)(rec + 0x6C); c.edx = 0x109;
    c.s[0] = pv; c.s[1] = unit; c.s[2] = r; c.s[3] = unit;
    h(&c);
    return 1;
}
