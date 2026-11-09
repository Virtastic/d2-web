// 1.13c D2Client+0xBE860 initUiColours: six D2Win #10190 palette-index lookups stored as bytes (+0x11BFF8, +0x11BFCC, +0x11C4B8/B9/BA); callees 0x8C6B0 and 0x265E0 between.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
struct D2cBE860G { uint8_t *bff8, *bfcc, *c4b8, *c4b9, *c4ba; };
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_be860(D2cHost h, const D2cBE860G* g) {
    auto pal = [&](uint32_t a, uint32_t b, uint32_t c) { D2cCall k = {}; k.rva = 0xD390; k.s[0] = a; k.s[1] = b; k.s[2] = c; return (uint8_t)h(&k); };
    D2cCall k = {}; k.rva = 0x8C6B0; h(&k);
    uint8_t a1 = pal(0xff, 0, 0); *g->bff8 = a1;
    uint8_t a2 = pal(0, 0, 0);    *g->bff8 = a2;
    uint8_t a3 = pal(0, 0, 0);    *g->bfcc = a3;
    uint8_t a4 = pal(0xff, 0, 0); *g->c4b8 = a4;
    uint8_t a5 = pal(0xf4, 0xc0, 0x4c); *g->c4b9 = a5;
    k = {}; k.rva = 0x265E0; h(&k);
    *g->c4ba = pal(0, 0, 0xff);
}
