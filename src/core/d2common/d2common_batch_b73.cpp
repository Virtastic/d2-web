// 1.13c D2Common batch b73. Only +0x0D580 is plain; the other claimed rows were skipped:
// 0x1E920/0x59870 (global-table builders), 0x20E80/0x3C720 (hidden EBX/EBP/stack inputs),
// 0x42A70/0x3DB20 (calls through unpinned function-pointer globals).
#include <cstdint>
#include <cstring>
#include <initializer_list>

struct D2DynamicPathStrc;
__attribute__((weak)) uint32_t D2Common_D420_PathMoveCollision(D2DynamicPathStrc*, uint32_t, uint32_t);
__attribute__((weak)) void sub_6FDAB810(int*, int*);

template <class T> static T rd(const void* p, size_t o) { T v; std::memcpy(&v, (const char*)p + o, sizeof v); return v; }
template <class T> static void wr(void* p, size_t o, T v) { std::memcpy((char*)p + o, &v, sizeof v); }

// 1.13c D2Common+0x0D580: register map ESI = pPath; stack dx, dy (16.16 fixed), out[2], *pCount.
// Walks the step in whole-tile increments, collision-moving each; returns 1 on a blocked tile.
uint32_t sub_D580(void* pPathESI, int32_t dx, int32_t dy, int32_t* out, uint32_t* pCount)
{
    char* p = (char*)pPathESI;
    wr<uint32_t>(p, 0x1D4, 0);
    sub_6FDAB810(&dx, &dy); // EAX = &dx, ECX = &dy
    uint32_t x = rd<uint32_t>(p, 0), y = rd<uint32_t>(p, 4);
    const uint32_t flags = rd<uint32_t>(p, 0x34);
    const int32_t endX = (int32_t)(x + dx), endY = (int32_t)(y + dy);
    uint32_t prev = (x >> 16 & 0xffff) | (y >> 16) << 16;
    uint16_t cx = (uint16_t)(x >> 16);
    int32_t ny = (int32_t)y;
    for (;;) {
        if (cx == (uint16_t)((uint32_t)endX >> 16) && (uint16_t)(prev >> 16) == (uint16_t)((uint32_t)endY >> 16)) break;
        uint32_t nyU = (uint32_t)dy + (uint32_t)ny;
        uint16_t nx = (uint16_t)((uint32_t)dx + x >> 16), nyh = (uint16_t)(nyU >> 16);
        uint32_t cur = nx | (uint32_t)nyh << 16;
        if (cx != nx || (uint16_t)(prev >> 16) != nyh) {
            if (!D2Common_D420_PathMoveCollision((D2DynamicPathStrc*)p, cur, prev)) {
                out[0] = (int32_t)((x & 0xffff0000) + 0x8000);
                out[1] = (int32_t)(((uint32_t)ny & 0xffff0000) + 0x8000);
                *pCount = rd<uint32_t>(p, 0x1D4);
                return 1;
            }
            uint32_t n = rd<uint32_t>(p, 0x1D4);
            if (flags & 0x20000) wr<uint32_t>(p, 0x1D8 + n * 4, cur);
            wr<uint32_t>(p, 0x1D4, n + 1);
            if ((int32_t)(n + 1) > 9) break;
        }
        cx = nx; prev = cur; ny = (int32_t)nyU; x = (uint32_t)dx + x;
    }
    if (rd<uint32_t>(p, 0x1D4)) wr<uint32_t>(p, 0x34, rd<uint32_t>(p, 0x34) | 8);
    out[0] = endX; out[1] = endY;
    *pCount = rd<uint32_t>(p, 0x1D4);
    return 0;
}
