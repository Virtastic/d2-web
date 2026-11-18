// 1.13c D2Client+0x81750: the weather/overlay init. EAX = act (handed to 10459 twice); ret 0.
// Sets the rain timing words, fills five 12-entry colour tables through 10190, loads the four rain and four bubble overlays
// (wsprintfA "%s\UncompOverlays\<name>" with "DATA\GLOBAL", then 0x2B380, then 10086 on each handle) and clears +0x1138F4.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "d2client_81750.h"
const char* const d2c81750Fmt[8] = {
    "%s\\UncompOverlays\\Rain1", "%s\\UncompOverlays\\Rain2", "%s\\UncompOverlays\\Rain3", "%s\\UncompOverlays\\Rain4",
    "%s\\UncompOverlays\\bubble1", "%s\\UncompOverlays\\bubble2", "%s\\UncompOverlays\\bubble3", "%s\\UncompOverlays\\bubble4" };
const char d2c81750Dir[] = "DATA\\GLOBAL";
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
template <class T> void wr(uint8_t* p, uint32_t o, T v) { memcpy(p + o, &v, sizeof v); }
constexpr uint32_t B = 0x113880;   // blk[rva - B]
}
// 1.13c D2Client+0x81750
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_81750(D2c81750G* g, const D2c81750Env* e, int32_t act) {
    uint8_t* k = g->blk;
    e->f814f0(1);
    if (*g->once == 0) { e->f80be0(); *g->once = 1; }
    wr<int32_t>(k, 0x1138C4 - B, 0x1d4c); wr<int32_t>(k, 0x1138C8 - B, 0xfa); wr<int32_t>(k, 0x1138CC - B, 3000); wr<int32_t>(k, 0x1138D0 - B, 0x7d);
    if ((uint8_t)e->ord10459(act) == 2) wr<int32_t>(k, 0x1138CC - B, rd<int32_t>(k, 0x1138CC - B) * 3);
    if ((uint8_t)e->ord10459(act) == 4) wr<int32_t>(k, 0x1138CC - B, rd<int32_t>(k, 0x1138CC - B) * 3);
    wr<int32_t>(k, 0x1138B4 - B, 3000); wr<int32_t>(k, 0x1138AC - B, 0x1d4c); wr<int32_t>(k, 0x1138B0 - B, 0xfa); wr<int32_t>(k, 0x1138B8 - B, 0x32);
    for (int32_t i = 0, i9 = 0, i8 = 0, i6 = 0; i8 < 0x1e0; i++, i9 += 0x50, i8 += 0x28, i6 += 0x18) {
        const uint8_t b9 = (uint8_t)(i9 / 12), b8 = (uint8_t)(i8 / 12), b6 = (uint8_t)(i6 / 12);
        uint8_t c = (uint8_t)(0x62 - b9);
        k[0x1138D4 - B + i] = (uint8_t)e->ord10190(c, (uint8_t)(c + 0x19), c);
        c = (uint8_t)(0x2d - b8);
        k[0x1138A0 - B + i] = (uint8_t)e->ord10190(c, (uint8_t)(c + 0xa), c);
        c = (uint8_t)(0x19 - b6);
        k[0x1138F8 - B + i] = (uint8_t)e->ord10190(c, (uint8_t)(c + 5), c);
        c = (uint8_t)(b9 + 0x78);
        k[0x1138E0 - B + i] = (uint8_t)e->ord10190(c, c, c);
        c = (uint8_t)(b9 - 0x56);
        k[0x113940 - B + i] = (uint8_t)e->ord10190(c, c, c);
    }
    char buf[0x100]; uint8_t local[4];
    for (int s = 0; s < 2; s++) {
        const uint32_t h = s ? 0x113890 - B : 0x113904 - B, d = s ? 0x113880 - B : 0x11391C - B;
        for (int j = 0; j < 4; j++) { e->wsprintfA(buf, d2c81750Fmt[4 * s + j], d2c81750Dir); wr<uint32_t>(k, h + 4 * j, e->fdb380(buf, local, 0)); }
        for (int j = 0; j < 4; j++) wr<uint32_t>(k, d + 4 * j, e->ord10086(rd<uint32_t>(k, h + 4 * j)));
    }
    wr<int32_t>(k, 0x1138F4 - B, 0);
}
