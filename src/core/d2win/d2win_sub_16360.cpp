// src/core/d2win/d2win_sub_16360.cpp -- 1.13c D2Win+0x16360 (121 B), from the cached decompile.
// Timer-gated widget tick: type must be 8 (else fatal 0x6a). Returns 1 if the widget's flags (+8)
// hit bitMasks[0] and bitMasks[2]; on that path, if tick >= *nextTick, calls cb (+0x34) and sets
// *nextTick = tick + interval (+0x40).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2Win16360Env {
    void (*fatal)(uint32_t code);
    const uint32_t* bitMasks;   // gdwBitMasks
    uint32_t* nextTick;         // DAT_6f9aa674
    uint32_t tick;              // GetTickCount()
    void (*call)(uint32_t cb, uint8_t* w);  // cb invoked with &w
};

// 1.13c D2Win+0x16360
uint32_t D2Win_sub_16360(uint8_t* w, const D2Win16360Env& env) {
    uint32_t v[0x11]; std::memcpy(v, w, sizeof v);
    if (v[0] != 8) { env.fatal(0x6a); std::exit(-1); }
    if ((env.bitMasks[2] & v[2]) && (env.bitMasks[0] & v[2])) {
        if (*env.nextTick <= env.tick) {
            if (v[0xd]) env.call(v[0xd], w);
            *env.nextTick = v[0x10] + env.tick;
        }
        return 1;
    }
    return 0;
}
