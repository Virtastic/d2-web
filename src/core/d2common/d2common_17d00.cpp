// 1.13c D2Common+0x17D00 -- depth-first grid search with a cost bound.
// pPool: dword 0 = the start node, nodes of 40 bytes from +4, the used count at +0x8CA4 (at most 900).
// Node: +0 f, +4 h, +8 g, +0xC x, +0x10 y, +0x14 turns taken, +0x18 turn-stream pointer, +0x1C heading (0..3),
// +0x20 parent, +0x24 child. Returns the node standing on the target, or 0 when the bound or the pool runs out.
#include <cstdint>
#include <cstring>
#include "d2common_17d00.h"

typedef uintptr_t P;
P D2Common_17B70(P n, const P* head);
int D2Common_17C80(P pt, P obj, int32_t x, int32_t y, P list);
extern "C" int32_t d2_fn_d2common_3e230(int32_t eax, int32_t y1, int32_t y2, int32_t x2);

namespace {
const int8_t kStepX[4] = { 1, 0, -1, 0 };      // D2Common+0x51E4
const int8_t kStepY[4] = { 0, 1, 0, -1 };      // D2Common+0x51E0
const int32_t kDirHeading[25] = { 5, 4, 4, 4, 3, 6, 5, 4, 3, 2, 6, 6, 6, 2, 2, 6, 7, 0, 1, 2, 7, 0, 0, 0, 1 };   // D2Common+0xDC320, first dword of each 12-byte row
const uint8_t kTurns[16] = { 0, 1, 2, 3, 0, 1, 1, 1, 3, 2, 1, 2, 0, 3, 2, 1 };   // D2Common+0x8CAB8
inline int32_t& I(P b, uint32_t o) { return *reinterpret_cast<int32_t*>(b + o); }
inline uint8_t& U8(P b, uint32_t o) { return *reinterpret_cast<uint8_t*>(b + o); }
}

extern "C" const uint8_t* D2Common_17D00_TurnTable() { return kTurns; }

// 1.13c D2Common+0x17D00
extern "C" uint32_t D2Common_17D00_SearchPath(uint32_t pPool, int32_t nLimit, uint32_t pLevel, uint32_t pTarget)
{
    const P pool = pPool;
    P n = static_cast<P>(*reinterpret_cast<const uint32_t*>(pool));
    const int32_t tx = I(pTarget, 0), ty = I(pTarget, 4);
    for (;;) {
        if (I(n, 0xC) == tx && I(n, 0x10) == ty) return static_cast<uint32_t>(n);
        const uint8_t hd = U8(n, 0x1C);
        const int32_t y0 = I(n, 0x10);
        const int32_t nx = kStepX[hd & 3] + I(n, 0xC), ny = kStepY[hd & 3] + y0;
        bool take = false;
        int32_t g = 0, h = 0, f = 0;
        if (D2Common_17C80(pTarget, pLevel, nx, ny, n)) {
            const int32_t cost = (I(n, 0xC) == nx || y0 == ny) ? 2 : 3;
            g = I(n, 8) + cost;
            int32_t dx = nx - tx; if (dx < 0) dx = -dx;
            int32_t dy = ny - ty; if (dy < 0) dy = -dy;
            const int32_t lo = dx < dy ? dx : dy, hi = dx < dy ? dy : dx;
            h = lo + hi * 2;
            f = h + g;
            take = f <= nLimit;
        }
        if (!take) {
            const P head = static_cast<P>(*reinterpret_cast<const uint32_t*>(pool));   // 17B70 compares with the head dword (EDI = pool)
            n = D2Common_17B70(n, &head);
            if (!n) return 0;
            continue;
        }
        if (I(n, 0x24) == 0) {
            const int32_t cnt = I(pool, 0x8CA4);
            P c = 0;
            if (cnt != 900) {
                c = pool + 4 + static_cast<P>(cnt) * 40;
                I(pool, 0x8CA4) = cnt + 1;
                std::memset(reinterpret_cast<void*>(c), 0, 40);
            }
            I(n, 0x24) = static_cast<int32_t>(c);
            if (!c) return 0;
            I(c, 0x20) = static_cast<int32_t>(n);
        }
        const P p = n;
        n = static_cast<P>(static_cast<uint32_t>(I(p, 0x24)));
        I(n, 8) = g; I(n, 4) = h; I(n, 0) = f; I(n, 0x14) = 0;
        const int32_t d = d2_fn_d2common_3e230(tx, ty, ny, nx);
        const int8_t c2 = static_cast<int8_t>(kDirHeading[d] / 2);
        const P parent = static_cast<P>(static_cast<uint32_t>(I(n, 0x20)));
        const uint32_t k = static_cast<uint8_t>(U8(parent, 0x1C) - c2) & 3u;
        I(n, 0x18) = static_cast<int32_t>(reinterpret_cast<uintptr_t>(kTurns + k * 4));
        U8(n, 0x1C) = static_cast<uint8_t>(kTurns[k * 4] + c2) & 3;
        I(n, 0xC) = nx; I(n, 0x10) = ny;
    }
}
