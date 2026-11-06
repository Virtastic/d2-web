// 1.13c D2Common+0x41340 -- stdcall (level), ret 4. Walk the outdoor vertex ring (level+0x14 holds the outdoor, whose +0x64 is the ring head;
// vertex +0 x, +4 y, +0xC flags, +0x10 next): for each edge stamp the edge preset along it through 0x42A70 and OR bit 0 into the cell
// (cells +0x2C, row offsets +0x30) through the cell-OR pointer 0x6FDEB244 (0x36C10); flag bit 2 skips the walk, bit 1 ORs 0x400 into two
// cap cells; then the corner preset for the turn into the next edge. Level 0x6F ORs 0x400 into two cells by the frame's far corner.
// Level 0x75 picks the second preset column.
#include <cstdint>
#include <cstring>
#include "d2common_41340.h"

namespace {
// .rdata 0x5590..: corner table (0x5680), edge class (0x5734); 0xDCAA0..: preset rows (0xDCAB8, 0xDCAC8 stride 4, 0xDCB88 stride 2).
const int32_t k5590[] = { 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 373, 372, 372, 375, 0, 0, 373, 374, 374, 375, 1, 9, 9, -1, -1, 1, 8, -1, -1, 12, -1, -1, -1, -1, 12, 4, -1, -1, 5, 2, 2, 10, -1, -1, -1, -1, 10, 1, 9, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, 11, 3, 12, -1, -1, -1, -1, 12, 4, 4, 7, -1, -1, 2, 10, -1, -1, -1, -1, 10, -1, -1, 6, 3, -1, -1, 11, 11, 3, -1, 1, -1, 0, -1, 2, -1, 3, -1, 836, 836, 836, 836, 836, 836, 836, 861, 836, 836, 836, 858 };
const int32_t kDCAA0[] = { -1, 1, -1, 0, -1, -1, 50462976, 16843008, 33620483, 16909056, 0, 4, 364, 799, 16, 5, 365, 800, 17, 6, 366, 801, 0, 7, 367, 802, 18, 8, 368, 803, 19, 9, 369, 804, 22, 10, 370, 805, 0, 11, 371, 806, 0, 12, 372, 807, 23, 13, 373, 808, 0, 14, 374, 809, 0, 15, 375, 810, 881, 957, 882, 958, 883, 959, 884, 960, 885, 961, 886, 962, 887, 963, 888, 964, 889, 965, 890, 966, 891, 967, 892, 968 };
inline int32_t T5590(uint32_t rva, int32_t i) { return k5590[int32_t((rva - 0x5590) / 4) + i]; }
inline int32_t TDC(uint32_t rva, int32_t i) { return kDCAA0[int32_t((rva - 0xDCAA0) / 4) + i]; }
inline int32_t Rd(uint32_t a) { int32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
inline int32_t Sgn(int32_t v) { return v < 0 ? -1 : (v > 0 ? 1 : 0); }
inline int32_t Abs(int32_t v) { return v < 0 ? -v : v; }
inline int32_t Bump2(int32_t v) { return v < 0 ? v - 2 : (v > 0 ? v + 2 : v); }
void OrCell(uint32_t od, int32_t x, int32_t y, uint32_t m)
{
    const uint32_t c = uint32_t(Rd(od + 0x2C)) + uint32_t(Rd(uint32_t(Rd(od + 0x30)) + uint32_t(y) * 4) + x) * 4;
    uint32_t v; std::memcpy(&v, reinterpret_cast<void*>(uintptr_t(c)), 4); v |= m; std::memcpy(reinterpret_cast<void*>(uintptr_t(c)), &v, 4);
}
}

// 1.13c D2Common+0x41340
extern "C" void D2Common_41340_Port(uint32_t lv, const D2Common41340Env* env)
{
    const uint32_t od = uint32_t(Rd(lv + 0x14));
    const int32_t u = Rd(lv + 0x1D0) == 0x75;
    const int32_t col = u + 4;
    const uint32_t head = uint32_t(Rd(od + 0x64));
    uint32_t a = head, b = uint32_t(Rd(head + 0x10));
    bool more;
    do {
        const uint32_t bn = uint32_t(Rd(b + 0x10));
        const int32_t dx = Sgn(Rd(b) - Rd(a)), dy = Sgn(Rd(b + 4) - Rd(a + 4));
        const int32_t nx = Sgn(Rd(bn) - Rd(b)), ny = Sgn(Rd(bn + 4) - Rd(b + 4));
        const int32_t ex = Rd(b) & ~1, ey = Rd(b + 4) & ~1;
        int32_t x = Rd(a) & ~1, y = Rd(a + 4) & ~1;
        const int32_t cls = T5590(0x5734, dx + dy * 3);
        const int32_t preset = col < 4 ? TDC(0xDCAC8, col + cls * 4) : TDC(0xDCB88, u + cls * 2);
        const uint32_t fl = uint32_t(Rd(a + 0xC));
        if (!(fl & 2)) {
            while (x != ex || y != ey) {
                x += dx * 2; y += dy * 2;
                env->stamp(lv, x, y, preset, -1, 0);
                OrCell(od, x, y, 1);
            }
        }
        if (fl & 1) {
            const int32_t px = Rd(a), py = Rd(a + 4), qx = Rd(b), qy = Rd(b + 4);
            const int32_t cx = ((px > qx ? px : qx) - 4 * Abs(dx)) & ~1, cy = ((py > qy ? py : qy) - 4 * Abs(dy)) & ~1;
            OrCell(od, cx, cy, 0x400);
            OrCell(od, cx + 2 * Abs(dx), cy + 2 * Abs(dy), 0x400);
        }
        const int32_t k = T5590(0x5680, (Bump2(nx * 2) + ny * 2) * 9 + Bump2(dx * 2) + dy * 2);
        if (k != -1) {
            const int32_t cp = col < 4 ? TDC(0xDCAB8, col + k * 4) : TDC(0xDCB88, (u - 2) + k * 2);
            if (cp != 0) { env->stamp(lv, ex, ey, cp, -1, 0); OrCell(od, ex, ey, 1); }
        }
        more = b != head;
        a = b; b = bn;
    } while (more);
    if (Rd(lv + 0x1D0) == 0x6F) {
        const int32_t w = Rd(od + 0x5C), h = Rd(od + 0x60);
        OrCell(od, w - 2, h - 4, 0x400);
        OrCell(od, w - 2, h - 3, 0x400);
    }
}
