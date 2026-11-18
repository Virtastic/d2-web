// 1.13c D2Client+0x7BAD0 (104 B, ECX = node, ESI = point {x, y}): the dword +0x16 of the node is a state: 0 becomes 1, 3 answers 1.
// Then by the mode word [0x11C414] against half the width word [0xDBC48] (signed, truncating): mode 1 answers 1 when x > w/2 - 10,
// mode 2 when x < w/2 + 10, mode 3 always; every other path calls 0x7B240(node, x, y). Always answers 1.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2c_port_7bad0(uint8_t* node, const int32_t* p, uint32_t mode, int32_t width,
                                   void (*c7b240)(uint8_t* node, int32_t x, int32_t y)) {
    uint32_t st; memcpy(&st, node + 0x16, 4);
    if (st == 0) { st = 1; memcpy(node + 0x16, &st, 4); }
    else if (st == 3) return 1;
    int32_t half = width / 2;
    if (mode == 1) { if (half - 10 < p[0]) return 1; }
    else if (mode == 2) { if (p[0] < half + 10) return 1; }
    else if (mode == 3) return 1;
    c7b240(node, p[0], p[1]);
    return 1;
}
