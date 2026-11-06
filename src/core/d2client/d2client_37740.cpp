// 1.13c D2Client+0x37740: per-frame HUD gate, no args. The globals ride in the pinned dword array e.gl (index: 0 FAD90, 1 FAD8C, 2 FAD84, 3 FADB0, 4 FADDC,
// 5 FADE4, 6 11C454, 7 FAE08, 8 DBC48, 9 DBC4C, 10 11C304, 11 11C450, 12 11C300, 13 DD5D0). Any of the six busy flags nonzero: 11C454 := 1, return.
// Else 11C454 := 0 and 0x36450() decides: 0 = draw (18-dword zeroed local, 0x373C0, then two Ordinal_10041 records through 0xD096: six stack words
// (&local, x, y, -1, 5, 0), local[13] = 11C304 / 11C300 and local[0] = (11C450 != 0) on the second); nonzero = Ordinal_10227 (0xBF8C), the DD5D0 != 0x26
// fatal (0xBF92 cdecl 1 word, 0xBF80 cdecl 3 words, 0x3664(-1)), FAE08 := 0 and, when it was set, 0x3CC60(eax = 2, [D0B4 result, &0xD0960]).
#include "cl_env.h"
extern "C" void d2c_37740(const cl::Env& e) {
    auto G = [&](int i) { return e.R(e.gl + 4u * (uint32_t)i); };
    auto S = [&](int i, uint32_t v) { e.W(e.gl + 4u * (uint32_t)i, v); };
    for (int i = 0; i < 6; ++i) if (G(i)) { S(6, 1); return; }
    S(6, 0);
    if (!e.H(0x36450)) {
        int32_t l[18] = {};
        e.H(0x373c0);
        const uint32_t pl = (uint32_t)(uintptr_t)l;
        l[13] = (int32_t)G(10);
        e.H(0xd096, pl, G(8) - 0x4b, G(9) - 0xa0, 0xffffffffu, 5);
        if (e.blk) e.blk(e.ctx, 0xd096, pl, 18);
        const uint32_t c450 = G(11), c300 = G(12);
        l[13] = (int32_t)c300;
        l[0] = c450 != 0;
        e.H(0xd096, pl, G(8) - 0x48, G(9) - 0xa4, 0xffffffffu, 5);
        if (e.blk) e.blk(e.ctx, 0xd096, pl, 18);
        return;
    }
    const uint32_t old = G(7);
    e.H(0xbf8c);
    if (G(13) != 0x26) {
        const uint32_t msg = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0xB2000000u | 0xD359F, msg, 0x139b);
        e.H(0x3664, 0xffffffffu);
    }
    S(7, 0);
    if (old) {
        const uint32_t res = e.H(0xd0b4, 0xB2000000u | 0xD0960);
        e.H(0x3cc60, res, 0xB2000000u | 0xD0960, 0, 0, 0, 2);
    }
}
