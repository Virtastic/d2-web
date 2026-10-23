// src/core/d2win/d2win_f610.cpp -- 1.13c D2Win+0xF610 (157 B), from the cached decompile.
// Wheel dispatch: ctl in EAX, delta in ESI. ctl[0]==6 else fatal (code 0xE2). When (bit0 & ctl[2]) and
// (bit2 & ctl[2]) are set, ctl[0x11]==0, ctl[0x12]==0 and (delta>0 && flags8&8 || delta<0 && flags8&0x10),
// a zeroed 32-byte event is passed to the callback ctl[0xd] with DAT_6f9014a8 raised around the call. Returns 0.
#include <cstdint>
#include <cstring>
#include <cstdlib>

static uint32_t rd(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }

// 1.13c D2Win+0xF610
uint32_t D2Win_F610(const uint8_t* ctl, int32_t delta, uint32_t mask0, uint32_t mask2,
                    uint32_t* dat14a8, void (*cb)(uint8_t* ev), void (*fatal)(uint32_t code)) {
    if (rd(ctl) != 6) { fatal(0xE2); std::exit(-1); }
    uint32_t f = rd(ctl + 8), b = ctl[0x40];
    if ((mask0 & f) && (mask2 & f) && !rd(ctl + 0x44) && !rd(ctl + 0x48) &&
        ((delta > 0 && (b & 8)) || (delta < 0 && (b & 0x10))) && cb) {
        uint8_t ev[32] = {};
        ++*dat14a8; cb(ev); --*dat14a8;
    }
    return 0;
}
