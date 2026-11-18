// 1.13c D2Client+0x38BB0 -- key-bind dispatcher, the sibling of 0x38AD0 that matches on the word at msg+8 (ret 4).
// Nothing when msg+0xC has bit 0x40000000 (EAX stays as it came). Otherwise the first of the 114 bind entries (10 bytes, base
// 0x11A2E0) whose word at +4 equals msg+8, whose command (dword at +0) is not 0xFFFF, is below 0x39 (signed), at most 0xDF and has
// a nonzero handler word in the command table (0xF4F10, 12 bytes each) is taken. Key 0x73 with the high bit of GetKeyState(0x12)
// set ends it (EAX = that answer); so does mode 0x11C434 == 2 with a zero flag word (+8 of the command row; EAX = 0).
// IsBadCodePtr(handler) nonzero is the fatal path (line 0x80E, the process exits). The handler runs (ECX = 3 * command), then
// msg+0x18 = 1, msg+0x1C = 0 and 0xBE84(msg) is called. EAX at each exit is modelled: a scan that ends without a match leaves
// the last register value of the last entry (key word, command, or 3 * command).
#include "cl_pa4.h"
extern "C" uint32_t d2c_38bb0(uint32_t eax, uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (e.R(msg + 0xc) & 0x40000000u) return eax;
    const uint32_t key = e.R(msg + 8);
    uint32_t ent = kBase + 0x11A2E0;
    for (uint32_t i = 0; i < 114; ++i, ent += 10) {
        eax = e.R(ent + 4) & 0xffff;
        if (eax != key) continue;
        eax = e.R(ent);
        const int32_t id = (int32_t)eax;
        if (id == 0xffff || id >= 0x39 || id > 0xdf) continue;
        eax = 3u * (uint32_t)id;
        const uint32_t row = kBase + 0xF4F10 + 12u * (uint32_t)id;
        const uint32_t hnd = e.R(row);
        if (!hnd) continue;
        if ((e.R(ent + 4) & 0xffff) == 0x73) {
            eax = e.H(0xcf104, 0x12);
            if (eax & 0x8000u) return eax;
        }
        if (G(e, 0x11c434) == 2) {
            eax = e.R(row + 8);
            if (eax == 0) return 0;
        }
        if (e.H(0xcefcc, hnd)) {
            const uint32_t r = e.H(0xbf92, 0x80e);
            e.H(0xbf80, 0x6FB8359Fu, r, 0x80e);
            e.H(0x3664, 0xFFFFFFFFu);
        }
        e.H(hnd & 0xFFFFFF, 0, 0, 0, 0, 0, 0, 3u * (uint32_t)id);
        return fin(e, msg);
    }
    return eax;
}
