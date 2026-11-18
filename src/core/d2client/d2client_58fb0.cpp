// 1.13c D2Client+0x58FB0: the ground-item label pass (Alt held), from the disassembly. void(void); EAX at return is 0 when there
// is no player or no player room, the hover flag (1) once a label took the hover, else the result of the closing 0xD384 call.
// The rooms near the player's room (0xC3DC fills a list pointer and a count in caller-stack locals) are walked, and each room's
// unit chain (+0x74, next +0xE8): an item unit (type 4, flag bit 7 of +0xC8) while fewer than 0x20 labels exist gets a slot of the
// table at 0x11FEF8 (stride 0x120: l t r b, unit, text at +0x14, palette byte +0x114, draw mode +0x118, colour +0x11C).
// Its screen point is the projected position (perspective flag from 0xD07E: 0xD09C/0xD084 with x,y out-locals and a +-W/4 shift
// under side panel 1/2; flat: 0xC1D2/0xC1F6 minus the camera globals). Inside the panel-limited x range and [-8, H-0x10] in y the
// name is built (0xBFEC0, EAX = unit), measured (0xD3B4: width through EDX, height through stack word 0), placed by 0x58DE0 (up to
// 12 modes) and stored when it stays in range. The first label under the cursor (or the hovered unit from 0x51A80) takes mode 5
// with the palette byte from 0xD390; every other label is mode 1 with a colour from the quality (0xC19C), the grey rule and the
// quest-item rule. Then every slot is drawn (0xD3AE); no hover clears the two hover globals and calls 0xD384(0,0,0).
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t kT = 0xB2000000u;
constexpr uint32_t kCount = 0x117800, kSlots = 0x11FEF8, kStride = 0x120, kPlayer = 0x11BBFC, kCurY = 0x11B824, kCurX = 0x11B828;
constexpr uint32_t kCamY = 0x11995C, kCamX = 0x119960, kOffX = 0x11C418, kUi = 0x11C414, kScrW = 0xDBC48, kScrH = 0xDBC4C;
constexpr uint32_t kHov4 = 0x11C2F4, kHov8 = 0x11C2F8, kFile = kT | 0xD5990;
}
extern "C" uint32_t d2c_58fb0(const cl::Env& e) {
    const uint32_t persp = e.H(0xD07E);
    const uint32_t player = e.R(G(kPlayer));
    if (!player) return 0;
    const uint32_t room = e.H(0xC1F0, player);
    if (!room) return 0;
    const uint32_t hoverUnit = e.H(0x51A80);
    const int32_t curX = (int32_t)e.R(G(kCurX)), curY = (int32_t)e.R(G(kCurY));
    volatile uint32_t list = 0, nrooms = 0;
    e.H(0xC3DC, room, P(&list), P(&nrooms));
    const uint32_t pal = e.H(0xD390, 0, 0x40, 0x80) & 0xFF;
    uint32_t n = 0;
    e.W(G(kCount), 0);
    uint32_t hovered = 0;
    for (uint32_t ri = 0; ri < nrooms; ++ri) {
        for (uint32_t u = e.R(e.R(list + 4 * ri) + 0x74); u; u = e.R(u + 0xE8)) {
            if (!(e.R(u + 0xC8) >> 7 & 1) || e.R(u) != 4 || (int32_t)n >= 0x20) continue;
            const uint32_t slot = G(kSlots) + n * kStride, tslot = kT | (kSlots + n * kStride);
            e.W(slot + 0x10, u);
            int32_t x, y;
            volatile int32_t px = 0, py = 0;
            if (persp) {
                const uint32_t a = e.H(0xC1AE, u), b = e.H(0xC232, u);
                if (!e.H(0xD09C, a, b)) { n = e.R(G(kCount)); continue; }
                e.H(0xD084, a, b, 0, P(&px), P(&py));
                const uint32_t ui = e.R(G(kUi));
                const int32_t q = (int32_t)e.R(G(kScrW)) / 4;
                if (ui == 1) px = px - q; else if (ui == 2) px = px + q;
                x = px; y = py;
            } else {
                const int32_t cam = (int32_t)(e.R(G(kCamX)) - e.R(G(kOffX)));
                x = (int32_t)e.H(0xC1D2, u) - cam;
                const int32_t cy = (int32_t)e.R(G(kCamY)) - 8;
                y = (int32_t)e.H(0xC1F6, u) + (-0x10 - cy);
            }
            const int32_t w = (int32_t)e.R(G(kScrW));
            const uint32_t ui = e.R(G(kUi));
            int32_t minX = 0, maxX = w;
            if (ui == 1) maxX = w / 2 - 0x20; else if (ui == 2) minX = w / 2;
            volatile int32_t bottom = y;
            if (x < minX || x > maxX || y < -8 || y > (int32_t)e.R(G(kScrH)) - 0x10) { n = e.R(G(kCount)); continue; }
            e.W(slot + 0x14, e.R(slot + 0x14) & 0xFFFF0000u);
            e.H(0xBFEC0, tslot + 0x14, 0x100, 0, 0, 0, u);
            volatile int32_t top = 0, width = 0;
            e.H(0xD3B4, P(&top), 0, 0, 0, 0, 0, tslot + 0x14, P(&width));
            width = width + 8;
            volatile int32_t left = x - (width >> 1);
            volatile int32_t right = width + left;
            top = y - top + 4;
            uint32_t ok = 0;
            for (uint32_t mode = 0; mode < 0xC; ++mode) {
                volatile uint32_t a[6] = { P(&left), P(&top), P(&right), P(&bottom), mode, (uint32_t)maxX };
                e.Xs((uint32_t)maxX);
                const uint32_t r = e.H(0x58DE0, a[0], a[1], a[2], a[3], mode, 0, (uint32_t)minX);
                if (e.blk) e.blk(e.ctx, 0x58DE0, P(a), 6);
                if (r) { ok = 1; break; }
            }
            if (right > maxX || left < minX || !ok) { n = e.R(G(kCount)); continue; }
            e.W(slot + 0, (uint32_t)left); e.W(slot + 4, (uint32_t)top); e.W(slot + 8, (uint32_t)right); e.W(slot + 0xC, (uint32_t)bottom);
            bool plain = hovered != 0;
            if (!plain) {
                if (curX >= left && curX <= right && curY >= top && curY <= bottom + 4) {
                    e.W(slot + 0x11C, 0); e.W(slot + 0x114, pal); e.W(slot + 0x118, 5);
                    e.H(0x51E30, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
                    hovered = 1;
                } else if (u == hoverUnit) {
                    e.W(slot + 0x114, pal); e.W(slot + 0x118, 5); e.W(slot + 0x11C, 0);
                    e.H(0xD384, 0, 0, 0);
                    e.W(G(kHov8), 0); e.W(G(kHov4), 0);
                    hovered = 1;
                } else plain = true;
            }
            if (plain) {
                e.W(slot + 0x114, 0); e.W(slot + 0x118, 1);
                uint32_t c = 0;
                switch (e.H(0xC19C, u)) {
                case 4: c = 3; break; case 5: c = 2; break; case 6: c = 9; break;
                case 7: c = 4; break; case 8: c = 8; break; case 9: c = 10; break;
                default:
                    if ((e.H(0xC1A8, u, 0x800, 0x106, kFile) && e.H(0xC90A, u) && (e.H(0xC97C, u) & 0xFF)) ||
                        e.H(0xC1A8, u, 0x400000, 0x10C, kFile))
                        c = 5;
                }
                const uint32_t rec = e.H(0xC1BA, e.R(u + 4));
                if ((e.R(rec + 0x12A) & 0xFF) && e.R(rec + 0x80) != 0x2067656Cu) c = 4;
                e.W(slot + 0x11C, c);
            }
            n = e.R(G(kCount)) + 1;
            e.W(G(kCount), n);
        }
    }
    if ((int32_t)n > 0) {
        uint32_t i = 0;
        do {
            const uint32_t s = G(kSlots) + i * kStride, ts = kT | (kSlots + i * kStride);
            e.H(0xD3AE, e.R(s + 0xC), e.R(s + 0x114), e.R(s + 0x118), e.R(s + 0x11C), 0, 0, ts + 0x14, e.R(s));
            ++i;
        } while ((int32_t)i < (int32_t)e.R(G(kCount)));
    }
    if (hovered) return hovered;
    e.W(G(kHov8), 0); e.W(G(kHov4), 0);
    return e.H(0xD384, 0, 0, 0);
}
