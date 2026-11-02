// src/core/d2client/b60d_seam.h -- the D2Client 0x8B840..0x8BD90 globals and callee seam (shared with its probes).
#pragma once
#include <cstdint>

struct B60dEnv {
    int32_t *g_bb35c /*1BBB35C*/, *g_cbc6c /*1CBC6C*/, *g_cbc70 /*1CBC70*/, *g_cbc80 /*1CBC80*/;
    const int32_t *g_c9854 /*1C9854*/;
    void (*o10065)(int32_t); void (*o10043)(int32_t, int32_t);
    uint32_t *hover[6] /*B90EA8..B90EBC*/; uint32_t *z[4] /*1CBC28,2C,38,3C*/;
    const int32_t *gW /*8BC48*/, *gH /*8BC4C*/, *offX /*1CB9A0*/, *offY /*1CB9A4*/;
};
