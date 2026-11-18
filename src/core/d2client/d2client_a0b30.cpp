// 1.13c D2Client+0xA0B30: EAX = unit, stack outX, outY (ret 8). Position of the unit plus a facing-direction step.
// Step table on the stack: x = T[dir], y = T[dir + 8], T = {0,-1,-2,-1,0,1,2,1, 2,1,0,-1,-2,-1,0,1}; dir = (C610)(unit) & 0xFF) >> 3.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cA0b30Env {
    uint32_t (*ord_c610)(uint32_t unit);   // thunk C610: facing byte in AL
    uint32_t (*ord10686)(uint32_t path);   // thunk C3BE: path x
    uint32_t (*ord10476)(uint32_t path);   // thunk C2EC: path y
};
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_a0b30(const D2cA0b30Env* e, const uint8_t* unit, uint32_t unitH, uint32_t* outX, uint32_t* outY) {
    static const int32_t T[16] = {0, -1, -2, -1, 0, 1, 2, 1, 2, 1, 0, -1, -2, -1, 0, 1};
    uint32_t dir = (e->ord_c610(unitH) & 0xff) >> 3;
    int32_t t; uint32_t p; memcpy(&t, unit, 4); memcpy(&p, unit + 0x2c, 4);
    bool plr = t == 2 || (t > 3 && t <= 5);
    uint32_t a, b = 0;
    if (plr) memcpy(&a, (const uint8_t*)(uintptr_t)p + 0xc, 4); else a = p ? e->ord10686(p) : 0;
    *outX = (uint32_t)T[dir] + a;
    if (plr) memcpy(&b, (const uint8_t*)(uintptr_t)p + 0x10, 4); else b = p ? e->ord10476(p) : 0;
    *outY = (uint32_t)T[dir + 8] + b;
}
}
