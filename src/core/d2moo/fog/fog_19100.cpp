// 1.13c Fog+0x19100 (ordinal 10029): cdecl logger(fmt, ...). A NULL format is the fatal path (not framed: it ends in _exit(-1));
// otherwise the default channel (module global Fog+0x268EC, passed as 0xB2000000|rva as the capture logs it) and the variadic words
// go to the log writer Fog+0x18770(channel, 1, 1, fmt, &variadic) through the FogEnv seam.
#include <cstdint>
#include "fog_env.h"
extern "C" void fog_19100(const FogEnv* e, uint32_t fmt, const uint32_t* va) {
    if (!fmt) return;
    fe_call(e, 0x18770, 0, 0, 0, 0, 0, 0xB2000000u | 0x268ECu, 1, 1, fmt, (uint32_t)(uintptr_t)va);
}
