// D2Client sub_A5A60: "find room for unit" assert. Globals passed in (each names its RVA); callees via D2cHost.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
namespace {
inline u32 go(D2cHost h, u32 rva, u32 a = 0, u32 s0 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = a; c.ecx = a; c.edx = a; c.s[0] = s0; return h(&c);
}
}
extern "C" {
// 1.13c D2Client+0xA5A60  (nX = ecx, nY = eax; ctl = DAT_6fbcbbfc, act = DAT_6fbcc3b8)
// Returns false where retail calls _exit(-1) after the assert text (Ordinal_578/10265/10024 are issued, then the caller aborts).
EMSCRIPTEN_KEEPALIVE bool d2c_a5a60(D2cHost h, u32 nX, u32 nY, u32 ctl, u32 act) {
    if (nX == 0 && nY == 0) return true;
    if (ctl) {
        go(h, 0x80000000u | 10331, ctl);
        if (go(h, 0x1400) != 0) return true;
    }
    if (go(h, 0x80000000u | 11010, act, nX) != 0) return true;
    go(h, 0x80000000u | 10265, 0x13c);
    return false;
}
}
