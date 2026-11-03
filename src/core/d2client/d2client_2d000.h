// Seam for D2Client 0x2D000: the two D2Common stat thunks (low import thunks 0xC85C then 0xC7F0, four stack words each) plus the string seams.
#pragma once
#include "d2client_ustr_env.h"
struct D2c2D000Env {
    D2ClientUStrEnv u;
    uint32_t (*c85c)(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3);
    uint32_t (*c7f0)(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3);
};
