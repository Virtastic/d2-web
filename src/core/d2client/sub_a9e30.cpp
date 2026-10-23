// D2Client+0xA9E30: automap-style view setup: clip rect around the player, then walk the node list. Globals passed by pointer.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
inline u32 go(D2cHost h, u32 rva, u32 a = 0) { D2cCall c = {}; c.rva = rva; c.eax = a; c.ecx = a; c.edx = a; return h(&c); }
inline i32 rd(const void* p, u32 off) { i32 v; memcpy(&v, (const char*)p + off, 4); return v; }
}
extern "C" {
// 1.13c D2Client+0xA9E30
// state = *DAT_6fbcbbfc (+0 kind, +0x2c unit/path ptr); rect = DAT_6fbb4df0..fc (4 i32: l,t,r,b); count = DAT_6fbcbbf8; head = DAT_6fbcbbec.
// ordinals 10867/10750 = path x/y (unverified ordinals vs. callers' tables); EAX arg of 0xA9B50 = current node (UNVERIFIED: decompile shows extraout_EAX).
EMSCRIPTEN_KEEPALIVE void d2c_a9e30(D2cHost h, const uint8_t* state, i32* rect, i32* count, u32 head) {
    i32 k = rd(state, 0), p = rd(state, 0x2c);
    bool direct = k == 2 || (k > 3 && k < 6);
    i32 x = direct ? rd((const void*)(uintptr_t)p, 0xc) : (p ? (i32)go(h, 0x80000000u | 10867, p) : 0);
    i32 y = direct ? rd((const void*)(uintptr_t)p, 0x10) : (p ? (i32)go(h, 0x80000000u | 10750, p) : 0);
    rect[0] = x - 0x18; rect[1] = y - 0x18; rect[2] = x + 0x18; rect[3] = y + 0x18;
    go(h, 0xA9C90, 0); go(h, 0xA8880); go(h, 0xA8800);
    *count = 0;
    for (u32 n = head; n; ) { (*count)++; u32 cur = n; n = (u32)rd((const void*)(uintptr_t)n, 0x28); go(h, 0xA9B50, cur); }
}
}
