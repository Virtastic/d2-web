// 1.13c D2Win+0x08400 (sub_8400): set the two ints at +0xA0/+0xA4 of a type-4 control.
// Fatal tail: Fog #10265(id) -> #10024("", text), then _exit(-1).
#include <cstdint>
#include <unistd.h>

struct D2Win8400Env { void (*fatal)(uint32_t stringId); };

// 1.13c D2Win+0x08400. ctl = ECX, a = EDX, b = stack.
uint32_t d2win_sub_8400(int32_t* ctl, int32_t a, int32_t b, const D2Win8400Env& env) {
    uint32_t id = 0xfa;
    if (ctl) {
        if (*ctl == 4) { ctl[0x29] = b; ctl[0x28] = a; return 1; }
        id = 0xfb;
    }
    env.fatal(id);
    _exit(-1);
}
