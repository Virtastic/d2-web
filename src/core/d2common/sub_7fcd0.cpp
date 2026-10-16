// 1.13c D2Common+0x7FCD0 (#10407)
#include <cstdint>
static const int32_t kTbl[64] = {
    -1, -1, -1, 0, 2, 4, 6, 8,   -1, -1, 0, 1, 2, 4, 6, 8,   -1, 0, 0, 2, 3, 5, 7, 8,   0, 1, 2, 2, 4, 5, 7, 8,
    2, 2, 3, 4, 5, 6, 7, 9,      4, 4, 5, 5, 6, 7, 8, 9,      6, 6, 7, 7, 7, 8, 10, 10,  8, 8, 8, 8, 9, 9, 10, 11};
static int32_t Abs(int32_t v) { return v < 0 ? -v : v; }
// Distance metric over resolved unit fields (size category from 0x30F80, coordinates from the path): table lookup when both
// axes are < 8 and both sizes < 4 (-1 -> 0), else the 2:1 octile metric over the gaps minus the half-sizes.
int32_t D2Common_7FCD0_UnitDistance(int32_t x1, int32_t y1, int32_t s1, int32_t x2, int32_t y2, int32_t s2)
{
    // x1,y1 belong to param_1; the retail code subtracts param_1's coordinate from param_2's, then takes |.|.
    int32_t dx = Abs(x2 - x1), dy = Abs(y2 - y1);
    if (dx < 8 && dy < 8 && s1 < 4 && s2 < 4) {
        int32_t d = kTbl[dx + dy * 8];
        if (d < 0) return 0;
        if (s1 == 3 || s2 == 3) d--;
        if (d < 0) d = 0;
        if (s1 < 2 || s2 < 2) return d + 1;
        return d;
    }
    int32_t h = Abs((int32_t)((uint32_t)s1 >> 1) + (int32_t)((uint32_t)s2 >> 1));
    dx -= h; dy -= h;
    if (dx < 0) dx = 0;
    if (dy < 0) dy = 0;
    return dx <= dy ? dx + dy * 2 : dy + dx * 2;
}
