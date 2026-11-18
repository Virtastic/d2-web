// 1.13c D2Client+0x17500: step the unit (EAX) toward a target unit (s0): s1 = step multiplier, s2 = action ecx, s3 = block word 4, ret 0x10
#include "cl17.h"
extern "C" uint32_t d2c_17500(uint32_t unit, uint32_t tgt, uint32_t s1, uint32_t s2, uint32_t s3, const cl::Env& e) {
    uint32_t tx = cl17::posX(e, tgt), ty = cl17::posY(e, tgt);
    uint32_t sx = cl17::posX(e, unit), sy = cl17::posY(e, unit);
    uint32_t dx = 0, dy = 0;                       // sign of (self - target), unsigned compares as retail
    if (sx > tx) dx = 0xffffffffu; else if (sx < tx) dx = 1;
    if (sy > ty) dy = 0xffffffffu; else if (sy < ty) dy = 1;
    uint32_t path = e.R(tgt + 0x2c);
    uint32_t cd = e.H(0xcd24, path);
    uint32_t room = e.H(0xc1f0, tgt);
    if (e.H(0xc34c, room, dx + tx, dy + ty, 1, cd) & 0xffff) return 0;
    uint32_t a = s1 & 0xff;
    cl17::walk(e, tgt, path, a * dx + tx, a * dy + ty, s3, s2, tgt);
    return 1;
}
