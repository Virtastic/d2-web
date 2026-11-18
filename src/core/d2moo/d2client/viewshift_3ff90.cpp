#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x3FF90: panel-layout mode (EAX = mode), sets the shift globals and the register/stack args of the two callees.
// Disassembly: modes 0 and 3 store shift 0 to 0xCC418, mode 1 -(w/4), mode 2 +(w/4) (w = 0xDBC48, sar after the
// round-toward-zero bias); mode >= 4 leaves 0xCC418. Then 0xCC414 = mode and thunk 0xD1B0 (Ordinal_10006) runs with ECX = the shift.
// Then for mode < 4 only: 0x8A4D0 (ret 4) with EAX = [0xF9E18], ECX = p (mode 1: p - p/4, mode 2: p + p/4; p = [0xF9E14]), EDX = 0,
// ESI = [0x11C3C4], stack = 0 / -p/4 / p/4 / 0; mode 3 also zeroes 0x11C3DC/E0/F8/FC.
// out: [0]=0xCC418 after, [1]=0xCC414, [2]=ECX, [3]=EDX, [4]=stack, [5]=1 if mode 3 zeroing, [6]=EAX, [7]=ESI of the 0x8A4D0 call. Returns 1 if 0x8A4D0 is called.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3ff90(uint32_t mode, int32_t w, int32_t p, int32_t oldShift, int32_t a18, int32_t c3c4, int32_t* out) {
    int32_t shift = oldShift;
    int32_t q = (w + ((w >> 31) & 3)) >> 2;
    if (mode < 4) shift = (mode == 1) ? -q : (mode == 2) ? q : 0;
    out[0] = shift;
    out[1] = (int32_t)mode;
    int32_t a = (p + ((p >> 31) & 3)) >> 2;
    out[5] = 0; out[6] = a18; out[7] = c3c4; out[3] = 0;
    switch (mode) {
    case 0: out[2] = p; out[4] = 0; return 1;
    case 1: out[2] = p - a; out[4] = -a; return 1;
    case 2: out[2] = p + a; out[4] = a; return 1;
    case 3: out[2] = p; out[4] = 0; out[5] = 1; return 1;
    }
    return 0;
}
