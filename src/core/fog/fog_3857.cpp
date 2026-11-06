// 1.13c Fog+0x3857 ___updatetlocinfo(): __lock(0xC) (0x3CF4); r = ___updatetlocinfo_lk() (0x3796); unlock(0xC) (0x3C3F, reached through
// the function's own unlock stub at 0x3889); return r. The SEH prolog/epilog around it is x86 frame plumbing with no wasm counterpart.
#include <cstdint>
#include "fog_env.h"
extern "C" uint32_t fog_3857(const FogEnv* e) {
    fe_call(e, 0x3cf4, 0, 0, 0, 0, 0, 0xc);
    const uint32_t r = fe_call(e, 0x3796);
    fe_call(e, 0x3c3f, 0, 0, 0, 0, 0, 0xc);
    return r;
}
