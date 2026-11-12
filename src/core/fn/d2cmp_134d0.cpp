// d2cmp+0x134d0 DccBitRead: FUNCTION-mode probe.
// The vector records retail's 16-byte DccBits (4-byte links: start, cur, _8, pos at +0xc). The port's DccBits is native
// width (pos at +0x14), so the probe copies the blob into a native DccBits, calls the port, and writes cur, _8 and pos
// back in retail layout. The stream pointers are wasm addresses (low 32 bits), so start and cur read directly.
#include <emscripten.h>
#include <cstdint>
#include "../d2cmp/d2cmp_dcc.h"

static uint32_t rd32(uint32_t addr, uint32_t off) { return *(const uint32_t*)(uintptr_t)(addr + off); }
static void wr32(uint32_t addr, uint32_t off, uint32_t v) { *(uint32_t*)(uintptr_t)(addr + off) = v; }

// r[0] = n (EAX), r[1] = DccBits blob (EDX); r[4] = result
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_134d0(uint32_t* r) {
    const uint32_t blob = r[1];
    DccBits nb;
    nb.start = (const uint32_t*)(uintptr_t)rd32(blob, 0);
    nb.cur = (const uint32_t*)(uintptr_t)rd32(blob, 4);
    nb._8 = rd32(blob, 8);
    nb.pos = (int32_t)rd32(blob, 0xc);
    r[4] = d2cmp_134d0((int32_t)r[0], &nb);
    wr32(blob, 4, (uint32_t)(uintptr_t)nb.cur);
    wr32(blob, 8, nb._8);
    wr32(blob, 0xc, (uint32_t)nb.pos);
}
