// 1.13c D2Game+0x70010 -- copy a trade-item's properties: ESI = dst, EDI = src, stack = p1 (ret 4). A dst of kind 7 takes src's A and B values and
// then the mask 0x8000 with p1 (0xA65E); any other kind asks 0xA8A4(dst, 0x1000000, 0x9AA, file) and, if it answers, takes src's A value.
#include "d2game_70010.h"
// 1.13c D2Game+0x70010
void d2game_port_70010(uint32_t dst, uint32_t src, uint32_t p1, const D2Game70010Env* env)
{
    if (env->kind(dst) == 7) {
        env->setA(dst, env->getA(src));
        env->setB(dst, env->getB(src));
        env->setC(dst, 0x8000, p1);
        return;
    }
    if (env->check(dst, 0x1000000, 0x9aa)) env->setA(dst, env->getA(src));
}
