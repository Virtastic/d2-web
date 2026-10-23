// D2Client batch next-24: 0x420E0 0x42240 0x423A0 0x42850 0x42880. Globals are passed in (each names its RVA);
// callees go through D2cHost.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
inline u32 go(D2cHost h, u32 rva, u32 a = 0, u32 s0 = 0, u32 s1 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = a; c.ecx = a; c.edx = a; c.s[0] = s0; c.s[1] = s1; return h(&c);
}
}
extern "C" {
// 1.13c D2Client+0x420E0  (mouseWheelPresent = GetSystemMetrics(0x4b), 11C3CC, 119898, 119890)
EMSCRIPTEN_KEEPALIVE bool d2c_420e0(i32 wheel, i32 g3cc, i32 g898, i32 g890) { return wheel != 0 && g3cc != 0 && g898 == 1 && g890 == 0; }
// 1.13c D2Client+0x42240  (F4314, 11C568 count, 11B96C[] list; returns 1 if the fatal path (Ordinal_10265(0xa6), _exit) must run)
EMSCRIPTEN_KEEPALIVE i32 d2c_42240(i32 g4314, i32* cnt, i32* list) {
    if (g4314 != 5) return 0;
    if (*cnt > 4) return 1;
    list[*cnt] = 2; (*cnt)++; return 0;
}
// 1.13c D2Client+0x423A0  (now = 1197EC, last = 119818, 119808/980C/9810/9814 = out/acc pairs)
EMSCRIPTEN_KEEPALIVE void d2c_423a0(u32 now, u32* last, i32* o808, i32* a80c, i32* o810, i32* a814) {
    if (3000u < now - *last) { *o808 = *a80c / 3; *o810 = *a814 / 3; *last = now; *a80c = 0; *a814 = 0; }
}
// 1.13c D2Client+0x42850  (F8C9C=0, 11C404=1, then 0x14660(ecx=arg), 11C3A4=1)
EMSCRIPTEN_KEEPALIVE void d2c_42850(D2cHost h, u32 arg, i32* g8c9c, i32* g404, i32* g3a4) {
    *g8c9c = 0; *g404 = 1; go(h, 0x14660, arg); *g3a4 = 1;
}
// 1.13c D2Client+0x42880  (wheel = GetSystemMetrics(0x4b); g3cc=11C3CC g898=119898 g890=119890 g88c=11988C;
// strings F4748/F4754/F476C/F4784 handed to 0x0ECCC0(eax=Ordinal_10048(), ecx=str); msg = RegisterWindowMessage, found = FindWindow+SendMessage nonzero)
EMSCRIPTEN_KEEPALIVE void d2c_42880(D2cHost h, i32 wheel, i32 g3cc, i32 g898, i32 g890, i32 g88c, u32 msg, i32 found, i32* msgSlot) {
    auto call = [&](u32 str) { u32 o = go(h, 0x80000000u | 10048); go(h, 0xECCC0, o, str); };
    bool b45 = g88c == 4 || g88c == 5;
    if (wheel && !g3cc) goto tail;
    if (wheel) { if (g898 == 1 ? g890 != 0 : (g898 == 2 && b45)) call(0xF4748); }
    if (g3cc) {
        if (g898 == 1 || (g898 == 2 && b45)) call(0xF4754);
        if (g898 == 2 && g88c != 4 && g88c == 5) call(0xF476C);
    }
tail:
    if (wheel && g3cc && g898 == 1 && g890 == 0 && found && msg) { *msgSlot = (i32)msg; call(0xF4784); }
}
}
