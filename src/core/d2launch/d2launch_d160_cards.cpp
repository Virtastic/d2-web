// 1.13c D2Launch+0xD160 -- the character-select card controls. The build function registers eight D2Win type-4 controls from the
// table at image offset 0x227E0 (stride 0x30: type, x, y, w, h), and when Fog 10227 answers non-zero (the expansion game) eight
// more from 0x22960 (+0x180). D2Win places a control with its y as the BOTTOM edge. A press in either set selects that row
// (row = index in the set; D2Launch+0xE140). This is the hit test the play path asks.
#include <cstdint>

namespace {
struct Ctl { int32_t x, y, w, h; };
// type-4 descriptors: columns 37 and 309, row origins 178 + 93 * (i >> 1) (the 0x5D step D160 also writes to 0x25710).
constexpr Ctl primary(int i) { return { 37 + (i & 1) * 272, 178 + (i >> 1) * 93, 200, 92 }; }
// the second set, registered only when Fog 10227 is non-zero: x 237 / 509, 72 wide, 93 tall, same row origins.
constexpr Ctl extra(int i) { return { 237 + (i & 1) * 272, 178 + (i >> 1) * 93, 72, 93 }; }
bool inside(const Ctl& c, int32_t px, int32_t py) { return px >= c.x && px < c.x + c.w && py >= c.y - c.h && py < c.y; }
}

// Row whose control contains the 800x600 screen point, or -1.
extern "C" int32_t d2launch_card_at(int32_t px, int32_t py, int32_t expansion) {
    for (int i = 0; i < 8; i++) {
        if (expansion && inside(extra(i), px, py)) return i;
        if (inside(primary(i), px, py)) return i;
    }
    return -1;
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_launch_card_at(int32_t px, int32_t py, int32_t expansion) { return d2launch_card_at(px, py, expansion); }
#endif
