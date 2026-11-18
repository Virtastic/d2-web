// 1.13c D2Client+0xC2F40: the mini-panel toggle (no arguments, plain ret). Storm 423 SRegLoadValue("Diablo II", "Mini Panel", 1, &v)
// returning nonzero with v != 0 leaves it alone. Otherwise, with the mini panel hidden ([0xFADD4] == 0) and no blocking menu
// ([0x113A68] == 0), every open panel slot (flag table 0xFAD80, panel pointers 0xDD538, count [0xDD5D0] == 0x26) is closed by kind
// ([panel+0x54]: 1 = 0xc2790(i, 1, 0), 4 = 0x47B20 with EDI = [0x11971D] when [0x119721] is set, 2 / 3 / above 4 abandon the toggle).
// The flag flips unless the player unit ([0x11BBFC]) is a player that 0xA5440 reports busy, and the change is announced through
// 0x3CCC0 (shown) or 0x3CC60 (hidden) with D2Gfx 10048 and the descriptor at 0xD0864.
#include <cstdint>
namespace d2c_c2f40 {
using u32 = uint32_t;
struct Env {
    u32 (*g)(u32 rva); void (*sg)(u32 rva, u32 v);      // module globals by RVA
    u32 (*rd32)(u32 a);                                 // any other memory
    u32 mod;                                            // address tag of module RVA 0 (strings / descriptor passed by address)
    u32 (*storm423)(u32 key, u32 val, u32 flags, u32* out);   // 0xBE8A, stdcall 4
    void (*fog10227)();                                 // 0xBF8C
    void (*fatal)(u32 line);                            // 0xBF92 / 0xBF80 / exit(-1): never returns
    void (*fc2790)(u32 ecx, u32 edx, u32 s0);           // ret 4
    void (*f47b20)(u32 edi);
    u32 (*gfx10048)();                                  // 0xD0B4
    void (*f3ccc0)(u32 eax, u32 s0, u32 s1);            // ret 8
    void (*f3cc60)(u32 eax, u32 s0, u32 s1);            // ret 8
};
// Helper (EAX = unit, leaf): 1 when unit flag 0x10000 is set, or by type / mode (player 0 / 17, monster 0 / 12).
static u32 busy(const Env& e, u32 u) {
    if (e.rd32(u + 0xc4) & 0x10000) return 1;
    const u32 m = e.rd32(u + 0x10), t = e.rd32(u);
    if (t == 0) return m == 0 || m == 0x11;
    if (t == 1) return m == 0 || m == 0xc;
    return 0;
}
void c2f40(const Env& e) {
    u32 v = 0;
    if (e.storm423(e.mod + 0xd3588, e.mod + 0xd3594, 1, &v) && v) return;
    const u32 was = e.g(0xfadd4);
    e.fog10227();
    if (e.g(0xdd5d0) != 0x26) e.fatal(0x139b);
    u32 cur = e.g(0xfadd4);
    if (!cur) {
        if (e.g(0x113a68)) return;
        for (u32 i = 0; (int32_t)i < (int32_t)e.g(0xdd5d0); ++i) {
            if (e.g(0xfad80 + 4 * i) != 1) continue;
            const u32 k = e.rd32(e.g(0xdd538 + 4 * i) + 0x54);
            if (k == 1) e.fc2790(i, 1, 0);
            else if (k == 4) { if (e.g(0x119721)) e.f47b20(e.g(0x11971d)); }
            else if (k != 0) return;
        }
        cur = e.g(0xfadd4);
    }
    const u32 pl = e.g(0x11bbfc);
    if (!pl || (e.rd32(pl) == 0 && !busy(e, pl))) { cur = cur == 0; e.sg(0xfadd4, cur); }
    if (!was) { if (cur) e.f3ccc0(2, e.gfx10048(), e.mod + 0xd0864); }
    else if (!cur) e.f3cc60(2, e.gfx10048(), e.mod + 0xd0864);
}
} // namespace d2c_c2f40
