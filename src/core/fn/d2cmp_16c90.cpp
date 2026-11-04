// d2cmp+0x16c90 sub_16C90: FUNCTION-mode probe.
#include <emscripten.h>
#include <cstdint>
uint32_t d2cmp_16c90(const uint8_t* ctx, const uint8_t* edi, uint32_t* esi);

// r[0] = ctx (ECX), r[1] = header (EDI), r[2] = divisor out (ESI); r[4] = result
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_16c90(uint32_t* r) {
    r[4] = d2cmp_16c90((const uint8_t*)(uintptr_t)r[0], (const uint8_t*)(uintptr_t)r[1],
                       (uint32_t*)(uintptr_t)r[2]);
}
