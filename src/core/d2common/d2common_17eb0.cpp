// 1.13c D2Common+0x17EB0 -- link one outdoor path's two endpoints with a vertex chain.
// EBX = path index, stack = level (ret 4). Endpoints are outdoors+0xF8/+0xFC and +0x170/+0x174 (stride 0x14), in world
// units relative to the level origin (level+0x1C/+0x20), divided by 8 into grid cells. Adjacent cells get a two-vertex
// chain; otherwise the 0x17D00 search runs with a growing bound and the found node's parent chain becomes the vertices.
#include <cstdint>
#include <cstring>
#include "d2common_17d00.h"

extern "C" int32_t d2_fn_d2common_3e230(int32_t eax, int32_t y1, int32_t y2, int32_t x2);

namespace {
const int32_t kDirHeading[25] = { 5, 4, 4, 4, 3, 6, 5, 4, 3, 2, 6, 6, 6, 2, 2, 6, 7, 0, 1, 2, 7, 0, 0, 0, 1 };   // D2Common+0xDC320
inline int32_t& I(uintptr_t b, uint32_t o) { return *reinterpret_cast<int32_t*>(b + o); }
inline int32_t Div8(int32_t v) { return v / 8; }   // cdq; and edx,7; add; sar 3: truncation toward zero
uint32_t g_pool[0x8CA8 / 4];   // the search pool retail keeps in its 0x8D08-byte frame
uint32_t NewVertex(const D2Common17EB0Env* env, uint32_t memPool)
{
    const uint32_t v = env->alloc(memPool, 0x14, 0x13);
    std::memset(reinterpret_cast<void*>(static_cast<uintptr_t>(v)), 0, 0x14);
    return v;
}
}

// 1.13c D2Common+0x17EB0
extern "C" uint32_t D2Common_17EB0_BuildVertexPath(int32_t nIndex, uint32_t pLevel, const D2Common17EB0Env* env)
{
    const uintptr_t lv = pLevel;
    const uint32_t memPool = static_cast<uint32_t>(I(static_cast<uintptr_t>(static_cast<uint32_t>(I(lv, 0x1B4))), 0x478));
    const uintptr_t od = static_cast<uintptr_t>(static_cast<uint32_t>(I(lv, 0x14)));
    const int32_t lx = I(lv, 0x1C), ly = I(lv, 0x20);
    const uintptr_t e = od + static_cast<uintptr_t>(nIndex) * 0x14;
    const int32_t x1 = Div8(I(e, 0xF8) - lx), y1 = Div8(I(e, 0xFC) - ly);
    const int32_t x2 = Div8(I(e, 0x170) - lx), y2 = Div8(I(e, 0x174) - ly);
    int32_t& slot = I(od + static_cast<uintptr_t>(nIndex) * 4, 0x68);
    const int32_t dx = x1 - x2, dy = y1 - y2;
    const int32_t ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
    if (ax + ay < 2) {
        const uint32_t a = NewVertex(env, memPool);
        slot = static_cast<int32_t>(a);
        I(a, 0) = x1; I(a, 4) = y1;
        const uint32_t b = NewVertex(env, memPool);
        I(static_cast<uint32_t>(slot), 0x10) = static_cast<int32_t>(b);
        I(b, 0) = x2; I(b, 4) = y2;
        return 1;
    }
    const int32_t d = d2_fn_d2common_3e230(x2, y2, y1, x1);
    const int32_t lo = ax < ay ? ax : ay, hi = ax < ay ? ay : ax;
    const int32_t h = lo + hi * 2;
    uint32_t head[10] = {};
    head[0] = static_cast<uint32_t>(h); head[1] = static_cast<uint32_t>(h); head[2] = 0;
    head[3] = static_cast<uint32_t>(x1); head[4] = static_cast<uint32_t>(y1); head[5] = 0xFFFFFFFFu;
    head[6] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(D2Common_17D00_TurnTable()));
    head[7] = static_cast<uint32_t>((kDirHeading[d] / 2) & 3);
    int32_t target[2] = { x2, y2 };
    const uintptr_t pool = reinterpret_cast<uintptr_t>(g_pool);
    g_pool[0] = static_cast<uint32_t>(pool + 4);
    g_pool[0x8CA4 / 4] = 1;
    int32_t limit = h / 2 + h;
    const int32_t end = limit + 0x23;
    uint32_t found = 0;
    do {
        std::memcpy(g_pool + 1, head, sizeof head);
        found = D2Common_17D00_SearchPath(static_cast<uint32_t>(pool), limit, pLevel,
                                          static_cast<uint32_t>(reinterpret_cast<uintptr_t>(target)));
        limit += 5;
        if (static_cast<int32_t>(g_pool[0x8CA4 / 4]) >= 900) return 0;
        g_pool[0x8CA4 / 4] = 1;
        if (found) break;
    } while (limit < end);
    if (!found) return 0;
    uint32_t prev = 0;
    for (uintptr_t n = found; n; n = static_cast<uintptr_t>(static_cast<uint32_t>(I(n, 0x20)))) {
        const uint32_t v = NewVertex(env, memPool);
        if (slot == 0) slot = static_cast<int32_t>(v); else I(prev, 0x10) = static_cast<int32_t>(v);
        I(v, 0) = I(n, 0xC); I(v, 4) = I(n, 0x10);
        prev = v;
    }
    return 1;
}
