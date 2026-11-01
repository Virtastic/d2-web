// 1.13c D2Game+0x8E110 -- unit-type dispatch.
// ECX = object (fatal 0x712 if null), EAX = unit (fatal 0x713 if null), EDX = second argument, one stack argument (ret 4).
// Unit type 0 calls 0x8C910 (EAX = object; stack: unit, EDX, argument) and returns its result, type 1 calls 0x8D500 (stack: object)
// and returns its result, any other type returns 0.
#include "d2game_8d6_common.h"
struct D2Game8E110Env {
    uint32_t (*sub8C910)(uint32_t object, uint32_t unit, uint32_t edx, uint32_t arg);
    uint32_t (*sub8D500)(uint32_t object);
};
// 1.13c D2Game+0x8E110
uint32_t d2game_port_8e110(uint32_t object, uint32_t unit, uint32_t edx, uint32_t arg, const D2Game8E110Env* env)
{
    if (!object) d2g8::fatal(0x712);
    if (!unit) d2g8::fatal(0x713);
    const uint32_t type = d2g8::r32(unit, 0);
    if (type == 0) return env->sub8C910(object, unit, edx, arg);
    if (type == 1) return env->sub8D500(object);
    return 0;
}
