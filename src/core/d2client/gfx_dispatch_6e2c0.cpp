// src/core/d2client/gfx_dispatch_6e2c0.cpp -- 1.13c D2Client+0x6E2C0 (147 B), alpha lane, from
// build-wasm/ghidra-d2client/decomp/0x6E2C0.c. HIDDEN-REGISTER: EAX = in_eax (-1 selects the
// Ordinal_10350 stack out iStack_10). Ordinals and the 7-entry jump table PTR_LAB_6fb1e648 are host callbacks.
#include <cstdint>
#include <emscripten.h>

struct H6e2c0 {
    uint32_t (*ord10175)(uint32_t* p);
    // 10350(p, &out_a, &param2, &mode, file, line); out_a/iStack_10 is returned via *iout
    void (*ord10350)(uint32_t* p, int32_t* iout, uint32_t* p2, uint32_t* mode, const char* file, int line);
    uint32_t (*jump)(uint32_t mode);  // PTR_LAB_6fb1e648[mode]
};

// 1.13c D2Client+0x6E2C0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2client_6e2c0(const H6e2c0* h, uint32_t* p, uint32_t* rec, int32_t in_eax) {
    uint32_t mode = 6;
    if (!p) {
        rec[2] = 6;
    } else {
        uint32_t p2 = p[1];
        int32_t iout = 0;
        mode = h->ord10175(p);
        h->ord10350(p, &iout, &p2, &mode, "..\\Source\\D2Client\\ENGINE\\GfxUtil.cpp", 0x2f0);
        rec[2] = mode;
        rec[3] = p2;
        rec[4] = (uint32_t)(in_eax == -1 ? iout : in_eax);
        if (mode > 6) return 0;
    }
    return h->jump(mode);
}
