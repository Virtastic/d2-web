// 1.13c D2Client+0xC62E0, written from the disassembly. ECX = unit, one unused stack word (ret 4). 0x2A0B0(EAX = unit) first; a null path
// [unit+0x2C] is fatal (bf92(0x946), bf80(file tag, that), 3664(-1)) and the call carries on with 3664's return as the path. 10392(path) is
// the object (null: return 0); 10338(object) zero returns 0; else [unit+0xC4] |= 0x40, 0x4C500(EAX = 0, ECX = object; 5) and return 1.
#include "cl_mb2.h"
// 1.13c D2Client+0xC62E0
extern "C" uint32_t d2c_c62e0(uint32_t u, uint32_t, const clm::Env& e) {
    e.H(0x2a0b0, u);
    uint32_t path = e.R(u + 0x2c);
    if (!path) {
        const uint32_t s = e.H(0xbf92, 0, 0, 0, 0, 0, 0x946);
        e.H(0xbf80, 0, 0, 0, 0, 0, clm::Env::Mod(0xd359f), s);
        path = e.H(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
    }
    const uint32_t o = e.H(0xc2da, 0, 0, 0, 0, 0, path);
    if (!o) return 0;
    if (!e.H(0xcd1e, 0, 0, 0, 0, 0, o)) return 0;
    e.W(u + 0xc4, e.R(u + 0xc4) | 0x40);
    e.H(0x4c500, 0, o, 0, 0, 0, 5);
    return 1;
}
