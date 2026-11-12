// 1.13c D2Client+0x12150 -- message handler (stdcall, msg): busy gate; flag 0x40000000 in msg+0xC goes straight to the tail; type 0x1B
// clears the held flag 0x6FBA8BEC; any other type runs the table lookup (0xEA270) and keeps the flag for types 0xD, 0x2E, 8, 0x20
// when the lookup found a row, sets it otherwise. The out word is stamped with GetTickCount when its low word is nonzero.
#include "d2client_sn18.h"
extern "C" uint32_t d2c_12150(uint32_t msg, const cl::Env& e) {
    if (e.R(0x6fbcc3a4)) { e.W(msg + 0x18, 0); return msg; }
    uint32_t out = 0;
    if (!(e.R(msg + 0xc) & 0x40000000u)) {
        if (e.R(msg + 8) == 0x1b) {
            e.H(0x114F0, 1);
            e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
            e.W(0x6fba8bec, 0);
        } else {
            const uint32_t rec = e.R(e.R(0x6fba8b94) + e.R(0x6fbcc538) * 10);
            bool found = e.H(0xEA270, e.R(0x6fba8be8), sn18::R16(e, msg + 8), 0, 0, 0, (uint32_t)(uintptr_t)&out, rec) != 0;
            bool clear = false;
            if (found) {
                e.H(0x114F0, 1);
                e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 2);
                const uint32_t t = e.R(msg + 8);
                clear = t == 0xd || t == 0x2e || t == 8 || t == 0x20;
            }
            e.W(0x6fba8bec, clear ? 0 : 1);
            if (out & 0xffffu) {
                sn18::W16(e, 0x6fbcc54c, out);
                e.W(0x6fbcc550, e.H(sn18::kTick));
            }
        }
    }
    return sn18::tail(e, msg);
}
