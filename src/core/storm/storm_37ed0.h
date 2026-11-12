#pragma once
// 1.13c Storm+0x37ED0 (ordinal 312): the SCode display-string formatter. The .rdata tables it reads are seams
// (retail content stays out of the tree); the string helpers are the proven 0x3AFD0/0x3B080/0x3AE00 ports.
#include <cstdint>
#include "storm_batch_25220.h"

namespace storm_37ed0 {
struct Tables {
    const uint8_t* scd;    // 0x46940: the name templates, 16 bytes per (id>>2)&7 group; the control table follows at +0x80
    const uint8_t* ctl;    // 0x469C0: four control characters, indexed by a two-bit field of the id
    const uint8_t* xform;  // 0x46938: the transform appended when id bit 0x20 is set
    uint32_t prefix;       // 0x4692C: the output's first four bytes
    uint8_t prefixByte;    // 0x46930: the output's fifth byte
    void (*sub37E80)(int32_t* code, uint32_t* out);  // 0x37E80, EAX = code, EDX = out
};
// cdecl (id, dst, cap, flag): true iff the formatted string with its NUL fits in cap.
bool Storm_37ED0(storm25220::State& s, const Tables& t, uint32_t id, char* dst, uint32_t cap, uint32_t flag);
} // namespace storm_37ed0
