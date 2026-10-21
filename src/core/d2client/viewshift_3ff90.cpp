#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x3FF90: panel-layout mode (EAX = mode 0..3) -> shift globals + rect args for 0x8A4D0.
// Pure part: out[0]=DAT 0xBCC418 (shift), out[1]=0xBCC414 (mode), out[2..4]=args (p1,p2,p3) of 0x8A4D0,
// out[5]=1 if mode 3 (also zeroes 0xBCC3DC/3E0/3F8/3FC). Returns 0 for mode>=4 (no 0x8A4D0 call; 0xBCC418 untouched).
// w = DAT 0xB8BC48, p = DAT 0xBA9E14. Arithmetic >>2 with round-toward-zero as the decompile.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3ff90(uint32_t mode, int32_t w, int32_t p, int32_t* out) {
    int32_t shift = 0;
    int32_t q = (w + ((w >> 31) & 3)) >> 2;
    if (mode == 1) shift = -q; else if (mode == 2) shift = q;
    out[0] = (mode < 4) ? shift : 0;  // mode>=4: global untouched; caller keeps old value
    out[1] = (int32_t)mode;
    int32_t a = (p + ((p >> 31) & 3)) >> 2;
    out[5] = 0;
    switch (mode) {
    case 0: out[2] = p; out[3] = 0; out[4] = 0; return 1;
    case 1: out[2] = p - a; out[3] = 0; out[4] = -a; return 1;
    case 2: out[2] = p + a; out[3] = 0; out[4] = a; return 1;
    case 3: out[2] = p; out[3] = 0; out[4] = 0; out[5] = 1; return 1;
    }
    return 0;
}
