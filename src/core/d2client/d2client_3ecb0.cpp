// 1.13c D2Client+0x3ECB0 -- message handler (one stack word, the message, ret 4) that walks the 60 button rectangles of the
// panel strip against the cursor (x = 0x11B828, y = 0x11B824). Rectangle i is x in [word 0x119968+4i, +0x30] by y in
// [word 0x119A58+4i - 0x30, word 0x119A58+4i]. A hit asks the helper 0xC4EA (ECX = the local player 0x11BBFC, EDX = word
// 0x119B48+4i, stack word 0x119C38+4i) for an item; when there is one, 0xC7AE and 0xC766(item, 0, 0, 0, &count) are asked: if
// 0xC7AE does not answer -1 and 0xC766 answers nonzero with a count of 0 or less (signed) the cached slot 0xFAD8C, when set, is
// cleared (0xBF8C, the 0x26 check on 0xDD5D0, 0x3CC60 through the handle of string 0xD061C with EAX = 1) and the message is
// finished at once (a clear slot finishes it without the clear). Any other answer builds the 9-byte request {0x3C, dword 0x80000000 if
// 0x11C420 is set + word 0x119B48+4i, dword 0x119C38+4i}, sends it through 0x143E0, clears the cached slot the same way, clears
// 0x11C420, calls 0x42AD0 and marks the walk as hit. After the 60 rectangles, when the cursor y is at most the height word
// 0xDBC4C minus 0x31, nothing hit and the slot 0xFAD8C is set, 0xC2790 runs with ECX = 3, EDX = 1 and one stack zero. The message
// is finished (+0x1C = 0, +0x18 = 1, 0xBE84). With no local player, or a slot count other than 0x26, the assert path exits and
// is not captured.
#include <cstring>
#include "cl_pa4.h"
extern "C" uint32_t d2c_3ecb0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (G(e, 0x11bbfc) == 0) assertAt(e, 0x3ba);
    const uint32_t cy = G(e, 0x11b824);
    bool hit = false;
    for (uint32_t i = 0; i < 60; ++i) {
        const int32_t cx = (int32_t)G(e, 0x11b828), y = (int32_t)G(e, 0x11b824);
        const int32_t rx = (int32_t)G(e, 0x119968 + 4 * i), ry = (int32_t)G(e, 0x119a58 + 4 * i);
        if (cx < rx || cx > rx + 0x30 || y < ry - 0x30 || y > ry) continue;
        const uint32_t id = G(e, 0x119b48 + 4 * i), id2 = G(e, 0x119c38 + 4 * i);
        const uint32_t item = e.H(0xc4ea, id2, 0, 0, 0, 0, 0, G(e, 0x11bbfc), id);
        if (item == 0) continue;
        uint32_t cnt = 0;
        const uint32_t ca = (uint32_t)(uintptr_t)&cnt;
        bool freeOnly = false;
        if (e.H(0xc7ae, item) != 0xFFFFFFFFu) {
            if (e.H(0xc766, item, 0, 0, 0, ca) != 0 && (int32_t)e.R(ca) <= 0) freeOnly = true;
        }
        if (freeOnly) {
            const uint32_t old = G(e, 0xfad8c);
            if (old == 0) return fin(e, msg);
            e.H(0xbf8c);
            if (G(e, 0xdd5d0) != 0x26) assertAt(e, 0x139b);
            S(e, 0xfad8c, 0);
            const uint32_t hd = e.H(0xd0b4, 0xB2000000u | 0xD061C);
            e.H(0x3cc60, hd, 0, 0, 0, 0, 1);
            return fin(e, msg);
        }
        uint8_t buf[12] = {0};
        const uint32_t w1 = (G(e, 0x11c420) ? 0x80000000u : 0u) + id;
        buf[0] = 0x3c;
        std::memcpy(buf + 1, &w1, 4);
        std::memcpy(buf + 5, &id2, 4);
        e.H(0x143e0, (uint32_t)(uintptr_t)buf, 0, 0, 0, 0, 0, 0, 0, 9, 0);
        const uint32_t old = G(e, 0xfad8c);
        e.H(0xbf8c);
        if (G(e, 0xdd5d0) != 0x26) assertAt(e, 0x139b);
        S(e, 0xfad8c, 0);
        if (old) {
            const uint32_t hd = e.H(0xd0b4, 0xB2000000u | 0xD061C);
            e.H(0x3cc60, hd, 0, 0, 0, 0, 1);
        }
        S(e, 0x11c420, 0);
        hit = true;
        e.H(0x42ad0);
    }
    if ((int32_t)cy <= (int32_t)G(e, 0xdbc4c) - 0x31 && !hit && G(e, 0xfad8c) != 0) e.H(0xc2790, 0, 0, 0, 0, 0, 0, 3, 1);
    return fin(e, msg);
}
