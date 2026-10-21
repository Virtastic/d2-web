// Register args in retail: ECX=this (int x,y pair), EAX=list head, ESI=x, EBX=y, stack=param_1.
// Raw offsets: param_1+0x14 -> grid struct (+0x2c cells, +0x30 row offsets, +0x54/58 origin, +0x5c/60 size).
// List node: +0xC x, +0x10 y, +0x20 next.
#include <cstdint>

// 1.13c D2Common+0x17C80
extern "C" int d2_fn_d2common_17c80(const int32_t* pt, const uint8_t* list, int32_t x, int32_t y, const uint8_t* param1)
{
    if (x == pt[0] && y == pt[1]) return 1;
    const uint8_t* g = *(const uint8_t* const*)(param1 + 0x14);
    auto i = [&](int o) { return *(const int32_t*)(g + o); };
    if (i(0x54) <= x && i(0x58) <= y && x < i(0x5c) + i(0x54) && y < i(0x60) + i(0x58)) {
        const int32_t* rows = *(const int32_t* const*)(g + 0x30);
        const uint32_t* cells = *(const uint32_t* const*)(g + 0x2c);
        if ((cells[rows[y] + x] & 0x200) == 0) {
            for (;; list = *(const uint8_t* const*)(list + 0x20)) {
                if (!list) return 1;
                if (*(const int32_t*)(list + 0xc) == x && *(const int32_t*)(list + 0x10) == y) break;
            }
        }
    }
    return 0;
}
