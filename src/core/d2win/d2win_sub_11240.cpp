// 1.13c D2Win+0x11240 (sub_11240): control message dispatcher. Callees are seams (imported
// Ordinal_414/415/416/417 thunks at D2Win+0x7648..0x76A2; Fog #10265/#10024 fatal tail).
#include <cstdint>
#include <unistd.h>

struct D2Win11240Env {
    void (*o414)(void* ctl, uint32_t a, uint32_t b);
    void (*o415)(void* ctl, uint32_t a, uint32_t b);
    void (*o416)(void* ctl, uint32_t a, uint32_t b);
    void (*o417)(void* ctl, uint32_t a, uint32_t b);
    void (*fatal)(uint32_t stringId);                     // Fog #10265(id) -> #10024("", text)
};

// 1.13c D2Win+0x11240. ctl = ECX, kind = EDX, msg/param = stack.
void d2win_sub_11240(void* ctl, uint32_t kind, uint32_t msg, uint32_t param, const D2Win11240Env& env) {
    uint32_t id;
    if (ctl) {
        switch (kind) {
        case 0:
            if (msg == 0x111) id = 0x17;
            else if (msg == 0x101) id = 0x18;
            else if (msg == 0x100) id = 0x19;
            else { env.o417(ctl, msg, param); return; }
            break;
        case 1: env.o414(ctl, msg, param); return;
        case 2: env.o416(ctl, msg, param); return;
        case 3: env.o415(ctl, msg, param); return;
        default: return;
        }
    } else id = 0x13;
    env.fatal(id);
    _exit(-1);
}
