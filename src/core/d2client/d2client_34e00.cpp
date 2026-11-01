// Guard in front of the next helper (fastcall ecx, edx + five stack words, callee pops 20 bytes).
#include <cstring>
#include "d2client_34e00.h"
namespace d2client_34e00 {
// edx == 0 or a0 == null: eax is untouched (the caller's). rec = the record lookup(eax = (int16)*a0); null -> eax = 0. Otherwise
// the five stack words are re-pushed unchanged to the next helper (ecx, edx) and its eax is the result.
uint32_t sub_34E00(Ops& o, uint32_t eax0, uint32_t ecx, uint32_t edx, const uint8_t* a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
    if (!edx || !a0) return eax0;
    int16_t id; std::memcpy(&id, a0, 2);
    if (!o.c17F0(id)) return 0;
    return o.c34C00(ecx, edx, a0, a1, a2, a3, a4);
}
}
