// 1.13c D2Common+0x6CCF0: compile automap.txt into 32-byte records {levelType, tileType, style, start, end, cel[4], nCels},
// then the per-level-type [first, end) ranges. Callees go through D2Common6CCF0Env. Delta vs 1.10f DATATBLS_LoadAutomapTxt: the
// 1.13c loader takes the archive in EDX and 0x2C as a fifth stack word; the logic is the same.
#include <cstring>
#include "d2common_6ccf0.h"

namespace { template <class T> T* P(uint32_t a) { return reinterpret_cast<T*>(static_cast<uintptr_t>(a)); } }

// 1.13c D2Common+0x6CCF0
extern "C" void d2_seam_d2common_6ccf0(uint32_t arch, const D2Common6CCF0Env* e, D2Common6CCF0State* s)
{
    s->seedLo = 0; s->seedHi = 0x29a;
    uint32_t n = 0;
    const uint32_t rows = e->load(arch, &n);
    const int32_t cnt = (int32_t)n;
    const uint32_t out = e->alloc(n * 0x20);
    memset(P<void>(out), 0, n * 0x20);
    for (int32_t i = 0; i < cnt; ++i) {
        uint8_t* r = P<uint8_t>(out) + 0x20 * i;
        const uint32_t src = rows + 0x2c * i;
        const int32_t lv = e->levelIdx(src), tl = e->tileIdx(src + 0x10);
        memcpy(r, &lv, 4); memcpy(r + 4, &tl, 4);
        r[8] = P<uint8_t>(src)[0x18]; r[9] = P<uint8_t>(src)[0x19]; r[10] = P<uint8_t>(src)[0x1a];
        memcpy(r + 0xc, P<uint8_t>(src) + 0x1c, 16);
        int32_t k = 0;
        for (int j = 0; j < 4; ++j) { int32_t c; memcpy(&c, r + 0xc + 4 * j, 4); if (c == -1) break; ++k; }
        memcpy(r + 0x1c, &k, 4);
        if (k == 0) { e->fatal(0x51e); return; }
    }
    if (rows) e->freeFn(s->flag ? rows - 4 : rows, s->flag ? 0x8f6 : 0x8f8);
    s->pAutoMap = out; s->nAutoMap = n;
    memset(s->rand, -1, sizeof s->rand);
    int32_t i = 0;
    while (i < cnt) {
        int32_t lt; memcpy(&lt, P<uint8_t>(out) + 0x20 * i, 4);
        s->rand[lt][0] = i;
        int32_t j = i;
        while (j + 1 < cnt) { int32_t nx; memcpy(&nx, P<uint8_t>(out) + 0x20 * (j + 1), 4); if (nx != lt) break; ++j; }
        i = j + 1;
        s->rand[lt][1] = i;
    }
}
