// 1.13c D2Client+0x8C370: EAX = object. Class gate on the object's 4-byte class tag (+0x80)
// against the quest-state word at D2Client+0x11973B (unaligned), tested through D2Common's quest-flag getter (thunk 0xC58C, stdcall 3).
#include "cl_env.h"
extern "C" uint32_t d2cl_8c370(uint32_t obj, const cl::Env& e) {
    const uint32_t tag = e.R(obj + 0x80), g = e.R(0x6FAB0000u + 0x11973Bu);
    if (tag == 0x20737361u) return e.H(0xc58c, g, 9, 5) == 0;      // "ass "
    if (tag == 0x207a7978u) return e.H(0xc58c, g, 0x14, 5) == 0;   // "xyz "
    if (tag == 0x20327274u) {                                      // "tr2 "
        if (!e.H(0xc58c, g, 0x25, 8)) return 1;
        return e.H(0xc58c, g, 0x25, 7) ? 1 : 0;
    }
    return 0;
}
