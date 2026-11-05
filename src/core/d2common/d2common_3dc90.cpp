// 1.13c D2Common+0x3DC90: EAX = b, ECX = a. tgt = *(b + 0x20); table = *(a + 0x14) holds a signed count at +0x260 (reread every pass), list heads from +0x68
// and, in each node, the next link at +0x10. rect = { b[0x34] - 1, b[0x38] - 1, b[0x3C] + 3, b[0x40] + 3 }, a four-word stack array. For every node whose
// next link is non-null the seam 0x36CD0 runs with EAX = node, ESI = &rect and stack words tgt + 0x3C, 1, 0, 2 (ret 0x10).
#include <cstdint>
#include <cstring>

struct D2Common3DC90Env { void (*touch)(uint32_t node, const uint32_t* rect, uint32_t target3c); };   // 0x36CD0

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_3DC90_Raw(uint32_t b, uint32_t a, const D2Common3DC90Env* env)
{
    const uint32_t tgt = Rd(b + 0x20);
    const uint32_t tbl = Rd(a + 0x14);
    const uint32_t rect[4] = { Rd(b + 0x34) - 1u, Rd(b + 0x38) - 1u, Rd(b + 0x3C) + 3u, Rd(b + 0x40) + 3u };
    for (int32_t i = 0; i < static_cast<int32_t>(Rd(tbl + 0x260)); i++) {
        for (uint32_t n = Rd(tbl + 0x68 + 4u * static_cast<uint32_t>(i)); n != 0; n = Rd(n + 0x10)) {
            if (Rd(n + 0x10) != 0) env->touch(n, rect, tgt + 0x3C);
        }
    }
}
