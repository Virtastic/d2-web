// 1.13c D2Common+0x17180 -- stdcall (level), ret 4. Walk the outdoor border polygon (level+0x14 -> +0x64, a ring of
// vertices: +0 x, +4 y, +8 type byte, +0xC flags, +0x10 next) and stamp border tiles along each edge and at each
// corner through 0x42A70, OR-ing the edge mask into the cells (+0x2C cells, +0x30 row offsets) through the cell-OR
// pointer (0x36C10); act-specific features mid-edge; ends with 0x15840.
#include <cstdint>
#include <cstring>
#include "d2common_17180.h"

extern "C" void D2Common_15840_FlagCornerRuns(uint32_t pLevel);

namespace {
// .rdata 0x5590..0x5777: corner table (0x5680), edge class (0x5734), act feature tiles (0x55B8/0x55BC).
const int32_t k5590[] = { 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 373, 372, 372, 375, 0, 0, 373, 374, 374, 375, 1, 9, 9, -1, -1, 1, 8, -1, -1, 12, -1, -1, -1, -1, 12, 4, -1, -1, 5, 2, 2, 10, -1, -1, -1, -1, 10, 1, 9, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, 11, 3, 12, -1, -1, -1, -1, 12, 4, 4, 7, -1, -1, 2, 10, -1, -1, -1, -1, 10, -1, -1, 6, 3, -1, -1, 11, 11, 3, -1, 1, -1, 0, -1, 2, -1, 3, -1, 836, 836, 836, 836, 836, 836, 836, 861, 836, 836, 836, 858 };
// .rdata 0xDCAA0..: border tile sets (0xDCAB8 / 0xDCAC8 stride 4, 0xDCB88 stride 2).
const int32_t kDCAA0[] = { -1, 1, -1, 0, -1, -1, 50462976, 16843008, 33620483, 16909056, 0, 4, 364, 799, 16, 5, 365, 800, 17, 6, 366, 801, 0, 7, 367, 802, 18, 8, 368, 803, 19, 9, 369, 804, 22, 10, 370, 805, 0, 11, 371, 806, 0, 12, 372, 807, 23, 13, 373, 808, 0, 14, 374, 809, 0, 15, 375, 810, 881, 957, 882, 958, 883, 959, 884, 960, 885, 961, 886, 962, 887, 963, 888, 964, 889, 965, 890, 966, 891, 967, 892, 968 };
const int32_t kActFirst[] = { 1, 40, 75, 103, 109, 1024 };   // 0x4668: first level id of each act
inline int32_t T5590(uint32_t rva, int32_t i) { return k5590[int32_t((rva - 0x5590) / 4) + i]; }
inline int32_t TDC(uint32_t rva, int32_t i) { return kDCAA0[int32_t((rva - 0xDCAA0) / 4) + i]; }
inline int32_t Rd(uint32_t a) { int32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
inline uint8_t Rb(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }
inline int32_t Sgn(int32_t v) { return v < 0 ? -1 : (v > 0 ? 1 : 0); }
inline int32_t Abs(int32_t v) { return v < 0 ? -v : v; }
inline int32_t Bump2(int32_t v) { return v < 0 ? v - 2 : (v > 0 ? v + 2 : v); }

// 0x145A0 (ECX = level, stack type byte): tile-set column for the level's style (+0x1C0) and the vertex type.
int32_t TileColumn(uint32_t pLevel, uint8_t type)
{
    switch (Rd(pLevel + 0x1C0)) {
    case 2: return type == 0;
    case 0x10: return 2;
    case 0x1B: return 3;
    case 0x1F: return (Rd(pLevel + 0x1D0) == 0x75) + 4;
    default: return -1;
    }
}

uint32_t Cell(uint32_t od, int32_t x, int32_t y)
{
    return uint32_t(Rd(od + 0x2C)) + uint32_t(Rd(uint32_t(Rd(od + 0x30)) + uint32_t(y) * 4) + x) * 4;
}
void Or(uint32_t c, uint32_t m) { uint32_t v; std::memcpy(&v, reinterpret_cast<void*>(uintptr_t(c)), 4); v |= m; std::memcpy(reinterpret_cast<void*>(uintptr_t(c)), &v, 4); }
void AndNot(uint32_t c, uint32_t m) { uint32_t v; std::memcpy(&v, reinterpret_cast<void*>(uintptr_t(c)), 4); v &= ~m; std::memcpy(reinterpret_cast<void*>(uintptr_t(c)), &v, 4); }
}

// 1.13c D2Common+0x17180
extern "C" void D2Common_17180_DrawOutdoorBorder(uint32_t pLevel, const D2Common17180Env* env)
{
    const int32_t id = Rd(pLevel + 0x1D0);
    uint8_t act = 0;
    for (int32_t i = 1; i <= 5; ++i)
        if (id < kActFirst[i]) { act = uint8_t(i - 1); break; }
    const uint32_t od = uint32_t(Rd(pLevel + 0x14));
    const uint32_t head = uint32_t(Rd(od + 0x64));
    uint32_t cur = head, nxt = uint32_t(Rd(head + 0x10));
    for (;;) {
        const int32_t cx = Rd(cur), cy = Rd(cur + 4);
        const int32_t nx = Rd(nxt), ny = Rd(nxt + 4);
        const uint32_t nn = uint32_t(Rd(nxt + 0x10));
        int32_t dxA = Sgn(nx - cx), dyA = Sgn(ny - cy);
        const int32_t dxB = Sgn(Rd(nn) - nx), dyB = Sgn(Rd(nn + 4) - ny);
        const int32_t adx = Abs(dxA), ady = Abs(dyA);
        const int32_t len = Abs(adx != 0 ? cx - nx : cy - ny);
        uint8_t type = Rb(cur + 8);
        uint32_t mask = type != 0 ? 3u : 1u;
        int32_t col = TileColumn(pLevel, type);
        const int32_t cls = T5590(0x5734, dxA + dyA * 3);
        const int32_t tile = col < 4 ? TDC(0xDCAC8, col + cls * 4) : TDC(0xDCB88, col + cls * 2 - 4);
        const uint32_t curFlags = uint32_t(Rd(cur + 0xC));
        if (!(curFlags & 2)) {
            int32_t x = cx, y = cy;
            while (x != nx || y != ny) {
                y += dyA; x += dxA;
                env->stamp(pLevel, x, y, tile, -1, 0);
                Or(Cell(od, x, y), mask);
            }
        }
        if ((curFlags & 1) && !(curFlags & 2) && act <= 4) {
            const int32_t mx = (cx < nx ? cx : nx) + (len * adx) / 2;
            const int32_t my = (cy < ny ? cy : ny) + (len * ady) / 2;
            switch (act) {
            case 0: case 4:
                AndNot(Cell(od, mx, my), 0xF0000);
                Or(Cell(od, mx, my), id == 0x11 ? 0x40400u : 0x30400u);
                break;
            case 3:
                AndNot(Cell(od, mx, my), 0xF0000);
                Or(Cell(od, mx, my), 0x30400u);
                break;
            case 1: {
                const int32_t k = dxA + dyA * 2 + 2;
                env->stamp(pLevel, mx, my, T5590(0x55B8, k * 2), -1, 0);
                env->stamp(pLevel, mx + adx, my + ady, T5590(0x55BC, k * 2), -1, 0);
                break;
            }
            default: break;
            }
        }
        if (type != 0 || (type = Rb(nxt + 8)) != 0) mask |= 2;
        col = TileColumn(pLevel, type);
        const uint32_t nxtFlags = uint32_t(Rd(nxt + 0xC));
        int32_t idx;
        if (curFlags & 2) {
            int32_t b = Bump2(dxA), c;
            if (nxtFlags & 2) c = Bump2(dxB) + dyB;
            else c = Bump2(dxB * 2) + dyB * 2;
            idx = b + c * 9 + dyA;
        } else {
            int32_t b = Bump2(dxA * 2), c;
            if (nxtFlags & 2) c = Bump2(dxB) + dyB;
            else c = Bump2(dxB * 2) + dyB * 2;
            idx = c * 9 + b + dyA * 2;
        }
        int32_t corner = T5590(0x5680, idx);
        if (corner != -1) {
            int32_t t = col < 4 ? TDC(0xDCAB8, col + corner * 4) : TDC(0xDCB88, col + corner * 2 - 6);
            if (t == 0x13) t = Rb(cur + 8) == 1 ? 0x13 + (Rb(nxt + 8) != 1) : 0x15;
            if (t != 0) {
                env->stamp(pLevel, nx, ny, t, -1, 0);
                Or(Cell(od, nx, ny), mask);
            }
        }
        cur = nxt; nxt = nn;
        if (cur == head) break;
    }
    D2Common_15840_FlagCornerRuns(pLevel);
}
