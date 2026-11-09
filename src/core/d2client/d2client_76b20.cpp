// 1.13c D2Client+0x76B20: the party panel's mouse tooltips. No arguments, plain ret. Walks the panel rows (list head,
// next at +0xCD) beside the roster (skipping its first entry, next at +0x80). Over a row's name band it hands the row to
// the name tooltip; over one of the row's four buttons it draws the button's one- or two-line tooltip and stops.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd(uint32_t a, uint32_t o = 0) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline int32_t rs(uint32_t a, uint32_t o = 0) { return (int32_t)rd(a, o); }
}

// Addresses of the module globals (retail RVAs in the comments).
struct D2cGlobals76B20 {
    uint32_t curY, curX;      // 0x11B824 / 0x11B828: cursor
    uint32_t cfg;             // 0x11B980: game config (byte +0x1EB bit 2)
    uint32_t viewX, viewY;    // 0x11B9A0 / 0x11B9A4: panel origin
    uint32_t player;          // 0x11BBFC: local player unit
    uint32_t roster;          // 0x11BC14: roster list head
    uint32_t rows;            // 0x11BFD8: panel row list head
    uint32_t color;           // 0x11BFF8: text colour byte (written)
};

struct D2cSeam76B20 {
    uint32_t (*room)(uint32_t unit);                       // 0xC1F0 (stdcall 1)
    uint32_t (*color)(uint32_t, uint32_t, uint32_t);       // 0xD390 (0, 0, 0): low byte kept
    uint32_t (*roomTest)(uint32_t room);                   // 0xC208 (stdcall 1)
    uint32_t (*testA)(uint32_t eaxEntry);                  // 0x4DD20
    uint32_t (*testB)(uint32_t eaxEntry);                  // 0x4DD80
    uint32_t (*testC)(uint32_t eaxEntry);                  // 0x4DDD0
    uint32_t (*testD)(uint32_t eaxEntry);                  // 0x4DE20
    void (*nameTip)(uint32_t ebxEntry);                    // 0x763C0
    void (*frame)(uint32_t a, uint32_t b, uint32_t w, uint32_t two);   // 0x75220 (stdcall 4)
    int32_t (*center)(uint32_t id, uint32_t w);            // 0x75370 (stdcall 2)
    uint32_t (*str)(uint32_t ecxId);                       // 0xD1EC
    void (*text)(uint32_t ecxStr, int32_t edxX, int32_t y, uint32_t color, uint32_t z);   // 0xD37E (fastcall, 3 stack)
};

extern "C" void d2c_76b20(const D2cGlobals76B20* g, const D2cSeam76B20* s) {
    uint32_t node = rd(g->rows);
    const uint32_t r0 = rd(g->roster);
    uint32_t entry = r0 ? rd(r0, 0x80) : 0;
    const uint32_t room = s->room(rd(g->player));
    *(uint8_t*)(uintptr_t)g->color = (uint8_t)s->color(0, 0, 0);
    const uint32_t inGame = room && s->roomTest(room);
    if (!node) return;
    auto line = [&](uint32_t id, uint32_t w, int32_t y, uint32_t c) {
        const int32_t x = s->center(id, w);
        const int32_t vx = rs(g->viewX);
        s->text(s->str(id), vx + x, y - rs(g->viewY), c, 0);
    };
    auto pair = [&](uint32_t a, uint32_t b, uint32_t w) {
        s->frame(a, b, w, 1); line(a, w, 0x32, 0); line(b, w, 0x41, 0);
    };
    auto single = [&](uint32_t a, uint32_t w, uint32_t c) { s->frame(a, 0, w, 0); line(a, w, 0x37, c); };
    do {
        const int32_t vx = rs(g->viewX), cy = rs(g->curY), cx = rs(g->curX);
        const int32_t ry = rs(node, 0xC9) - rs(g->viewY);
        if (cx >= vx + 0x24 && cx <= vx + 0x8C && cy >= ry - 0x1A && cy <= ry) {
            s->nameTip(entry);
        } else {
            for (uint32_t k = 0; k < 4; k++) {
                const uint32_t bx = node + 0x24 + 0x20 * k;
                const int32_t x0 = vx + rs(bx);
                if (cx < x0 || cx > x0 + 0x14) continue;
                const int32_t y0 = rs(bx, 4) - rs(g->viewY);
                if (cy < y0 - 0x14 || cy > y0) continue;
                switch (rd(bx, 0x10)) {
                    case 1: {
                        const uint32_t ro = rd(g->roster);
                        if (!ro || *(const uint16_t*)(uintptr_t)(ro + 0x20) < 9 || rs(node, 0xBD) < 9) pair(0x101A, 0x101B, 0xA0);
                        else if (!inGame) single(0x1019, 0x6E, 1);
                        else if (s->testA(entry)) pair(0x100C, 0x100D, 0x6E);
                        else pair(0x100A, 0x100B, 0x6E);
                        return;
                    }
                    case 2:
                        if (!(*(const uint8_t*)(uintptr_t)(rd(g->cfg) + 0x1EB) & 4)) continue;
                        if (s->testD(entry)) pair(0x1010, 0x1011, 0xFF); else pair(0x100E, 0x100F, 0xFF);
                        return;
                    case 3:
                        if (s->testC(entry)) pair(0x1012, 0x1013, 0x114); else pair(0x1014, 0x1015, 0x114);
                        return;
                    case 4:
                        if (s->testB(entry)) single(0x1017, 0x128, 0); else single(0x1016, 0x128, 0);
                        return;
                    default: continue;
                }
            }
        }
        node = rd(node, 0xCD);
        if (entry) entry = rd(entry, 0x80);
    } while (node);
}
