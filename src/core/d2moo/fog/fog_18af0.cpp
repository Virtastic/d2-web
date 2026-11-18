// 1.13c Fog+0x18AF0 (ordinal 10238): cdecl logger(log, fmt, ...). A NULL format is the fatal path (not framed: _exit(-1)); a NULL log
// name selects the default channel (Fog+0x268EC, tagged 0xB2000000|rva); then the log writer Fog+0x18770(log, 0, 0, fmt, &variadic).
#include <cstdint>
#include "fog_env.h"
extern "C" void fog_18af0(const FogEnv* e, uint32_t log, uint32_t fmt, const uint32_t* va) {
    if (!fmt) return;
    if (!log) log = 0xB2000000u | 0x268ECu;
    fe_call(e, 0x18770, 0, 0, 0, 0, 0, log, 0, 0, fmt, (uint32_t)(uintptr_t)va);
}
