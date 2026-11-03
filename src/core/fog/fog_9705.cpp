// 1.13c Fog+0x9705 ___initmbctable. cdecl, no arguments; returns 0.
// guard (DAT_6ffa829c, RVA 0x5829C) clear: __setmbcp(-3), then the guard := 1. The probe passes the guard word and
// the __setmbcp seam (fog+0x95B5, a recorder in the capture).
#include <cstdint>

// 1.13c Fog+0x9705
extern "C" int32_t fog_initmbctable_9705(uint32_t* guard, void (*setmbcp)(int32_t)) {
    if (*guard == 0) {
        setmbcp(-3);
        *guard = 1;
    }
    return 0;
}
