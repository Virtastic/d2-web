// 1.13c D2Client+0x29990: item use dispatch (EAX flag, ECX token, EDX item, stack id; ret 4). The flag counts only while
// global 0x119854 is set. Nothing happens when #C202(token) or global 0xFADA4 is set. #C862 fills a stack record from
// #C8A4(item, 0, global 0x119854) and global 0x11B99C; #CF28(token, id) gives the object (zero leaves). An object whose
// +4 #C1BA rejects is fatal (string 0xA64, _exit). 0x29920(item, obj) must be 1 and 0x81CB0(obj) zero; then 0x148E0
// (AL = 0x26, ECX = obj+0xC, EDX = flag) and 0x81CE0. With flag and item, the first node of the list at global 0x11C4D4
// with +4 == 7, +0xC == item+0xC and +0x20 == 0 names (its +8, unless -1) the target of 0x23F20(DL = 1; 0x54 when any of
// stats 0x4C/0x51/0x50 is set, else 0x55); otherwise 0x88A70(EBX = 0x81D70(), 0, 0, 0, 0). Global 0x11C468 is cleared.
#include "d2client_callee.h"
// 1.13c D2Client+0x29990
extern "C" void d2c_29990(const D2cSvc& v, uint32_t flag /*EAX*/, uint32_t tok /*ECX*/, const uint32_t* item /*EDX*/, uint32_t id,
                          uint32_t g9854, uint32_t gada4, uint32_t gb99c, uint32_t* g468, const uint32_t* head /*[0x11C4D4]*/) {
    if (!g9854) flag = 0;
    const uint32_t it = (uint32_t)(uintptr_t)item;
    if (d2c_call(v, 0xc202, 0, 0, 0, 0, 0, tok) || gada4) return;
    uint32_t rec[6] = {};
    const uint32_t q = d2c_call(v, 0xc8a4, 0, 0, 0, 0, 0, it, 0, g9854);
    d2c_call(v, 0xc862, 0, 0, 0, 0, 0, q, gb99c, (uint32_t)(uintptr_t)rec);
    const uint32_t* obj = (const uint32_t*)(uintptr_t)d2c_call(v, 0xcf28, 0, 0, 0, 0, 0, tok, id);
    if (!obj) return;
    const uint32_t o = (uint32_t)(uintptr_t)obj;
    if (!d2c_call(v, 0xc1ba, 0, 0, 0, 0, 0, obj[1])) {
        const uint32_t s = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0xa64);
        d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xd359f, s);
        d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    }
    if (d2c_call(v, 0x29920, 0, 0, 0, 0, 0, it, o) != 1) return;
    if (d2c_call(v, 0x81cb0, o, 0, 0, 0, o)) return;
    d2c_call(v, 0x148e0, 0x26, obj[3], flag, 0, o, 0);
    d2c_call(v, 0x81ce0, o, 0, 0, 0, o);
    if (flag && item) {
        for (const uint32_t* n = head; n; n = (const uint32_t*)(uintptr_t)n[0x30 / 4]) {
            if (n[1] != 7 || n[3] != item[3] || n[8] != 0) continue;
            const uint32_t tg = n[2];
            if (tg == 0xffffffffu) break;
            const bool st = d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, o, 0x4c) || d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, o, 0x51) ||
                            d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, o, 0x50);
            d2c_call(v, 0x23f20, 0, tg, 1, 0, 0, st ? 0x54 : 0x55);
            *g468 = 0;
            return;
        }
    }
    const uint32_t r = d2c_call(v, 0x81d70, 0, 0, 0, 0, o);
    d2c_call(v, 0x88a70, r, 0, 0, r, 0, 0, 0, 0, 0);
    *g468 = 0;
}
