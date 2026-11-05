// Fog log-entry family over the FogEnv seam (the faithful twins of the log ports that write to stderr): Fog+0x190A0 (ordinal 10030, channel log) and
// Fog+0x18B50 (EAX = log name) hand the message to the log writer Fog+0x18770(channel, flag, flag, fmt, &variadic words) through fe_call, the
// default channel being the module global at Fog+0x268EC (passed as 0xB2000000|rva, as the capture logs it). The NULL-format fatal path
// (0xFC20, Ordinal_116, 0x10F30, 0x184A0, _exit(-1)) never returns in retail and is not framed.
#include <cstdint>
#include "fog_env.h"

namespace { constexpr uint32_t MOD = 0xB2000000u, CHAN = MOD | 0x268ECu; }

extern "C" {
// 1.13c Fog+0x190A0 (cdecl channel, fmt, ...); va = address of the first variadic word
void fog_190a0(const FogEnv* e, uint32_t ch, uint32_t fmt, const uint32_t* va) {
    if (!fmt) return;
    if (!ch) ch = CHAN;
    fe_call(e, 0x18770, 0, 0, 0, 0, 0, ch, 1, 1, fmt, (uint32_t)(uintptr_t)va);
}
// 1.13c Fog+0x18B50 (EAX = log name, stack fmt, ...)
void fog_18b50(const FogEnv* e, uint32_t name, uint32_t fmt, const uint32_t* va) {
    if (!fmt) return;
    if (!name) name = CHAN;
    fe_call(e, 0x18770, 0, 0, 0, 0, 0, name, 1, 0, fmt, (uint32_t)(uintptr_t)va);
}
}
