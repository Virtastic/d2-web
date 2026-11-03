// 1.13c D2Client+0x68B80: ESI = string. 8+ chars is fatal; else hash = Ordinal_502(str, 1, 0)
// folded as ((h>>8^h)>>16 ^ h>>8 ^ h) & 0x1FF.
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_68b80(uint32_t str, const Env* e) {
    uint32_t n = 0;
    while (B(str, n)) ++n;
    if (n >= 8) { e->fatal(0x79); return 0; }
    uint32_t h = e->call4(0xBF0E, 0, 0, 0, 0, 0, str, 1, 0, 0);
    uint32_t t = h >> 8;
    return (((t ^ h) >> 16) ^ t ^ h) & 0x1FF;
}
