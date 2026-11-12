// 1.13c D2Client+0x728B0: PlrSkills.cpp line 0x26, the skill-hook gate run by 666D0 (EAX = in_EAX, EBX = unaff_EBX, passed on).
// Callees go through D2cHost (rva | 0x80000000 = ordinal). The sgptDataTables skills table (count @+0xBA0, base @+0xB98, stride 0x23C,
// short @+0x2E) is read through host globals: rva 0x40000000|0xFFFF0BA0 = count, 0x40000000|0xFFFF0B98 (eax = index) = record, 0x40000000|0xFFFF002E (eax = record) = the short.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32; typedef int32_t i32;
static u32 call(D2cHost h, u32 rva, u32 eax = 0, u32 ebx = 0, u32 s0 = 0, u32 s1 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = eax; c.edx = eax; c.ebx = ebx; c.s[0] = s0; c.s[1] = s1; return h(&c);
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_728b0(D2cHost h, u32 eax, u32 ebx) {
    u32 p = call(h, 0x80000000u | 10511);
    if (!p) return;
    if (!call(h, 0x724E0)) return;
    i32 i = (i32)call(h, 0x80000000u | 10601, p, 0, 0, 0x26);
    if (i < 0 || (u32)i >= call(h, 0x40000000u | 0xFFFF0BA0u)) return;
    u32 rec = call(h, 0x40000000u | 0xFFFF0B98u, (u32)i);
    if (!rec) return;
    i32 t = (int16_t)(call(h, 0x40000000u | 0xFFFF002Eu, rec) & 0xFFFF);
    if (t == 0x43) { if (call(h, 0x80000000u | 10397, p) & 1) call(h, 0x2B960, eax, ebx, 1); }
    else if (t == 0x4c) call(h, 0xC5E50, eax, ebx, ebx, 1);
}
