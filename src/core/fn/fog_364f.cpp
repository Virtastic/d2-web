// 1.13c Fog+0x364F ___isctype_mt: single-byte path (c in [-1, 0xFF]) of the
// ported ___isctype_mt in src/core/d2launch/d2launch_crt_n21.cpp; r[0] = retail locale block (ctype1 ptr at +0x48),
// r[1] = c, r[2] = mask. The DBCS branch is not framed (it calls GetStringTypeA), so the callback is null here.
#include <emscripten.h>
#include <cstdint>
#include <cstdbool>
uint32_t ___isctype_mt(const uint16_t* ctypeTab, const uint8_t* lead, uint32_t cp, uint32_t lcid, int c, uint32_t mask,
                       bool (*getStringType)(const uint8_t*, int, uint16_t*, uint32_t, uint32_t));
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_364f(uint32_t* r) {   // r[0..2] args in, r[4] result out
    const uint16_t* tab = (const uint16_t*)(uintptr_t)*(const uint32_t*)(uintptr_t)(r[0] + 0x48);
    r[4] = ___isctype_mt(tab, nullptr, 0, 0, (int)r[1], r[2], nullptr);
}
