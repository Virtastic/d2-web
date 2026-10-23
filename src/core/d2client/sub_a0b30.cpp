// D2Client sub_A0B30: unit facing-direction step offset + position -> (x,y).
#include <cstdint>
#include <cstring>

struct A0B30Env {
    uint32_t (*getDirection)();                   // D2Common ordinal 10663 (no args in decompile)
    uint32_t (*getPathX)(uint32_t path);          // ordinal 10867
    uint32_t (*getPathY)(uint32_t path);          // ordinal 10750
    const uint8_t* (*pathBytes)(uint32_t path);   // host view of the 32-bit path struct
};
static A0B30Env a0b30_env;
extern "C" void a0b30_set_env(const A0B30Env* e) { a0b30_env = *e; }

// 1.13c D2Client+0xA0B30  (EAX = unit bytes; outX/outY on stack)
// ponytail: Ghidra shows the second store hitting outX again (register/stack mixup); read as outY with t[dir+1].
extern "C" void a0b30_step(const uint8_t* unit, uint32_t* outX, uint32_t* outY) {
    int32_t t[10] = {1, 2, 1, 0, -1, -2, -1, 0, 1, 0};
    uint32_t dir = (a0b30_env.getDirection() & 0xff) >> 3;
    uint32_t type, path;
    memcpy(&type, unit, 4);
    memcpy(&path, unit + 0x2c, 4);
    bool direct = type == 2 || (type > 3 && type < 6);
    uint32_t x = 0, y = 0;
    if (direct) {
        memcpy(&x, a0b30_env.pathBytes(path) + 0xc, 4);
        memcpy(&y, a0b30_env.pathBytes(path) + 0x10, 4);
    } else if (path) {
        x = a0b30_env.getPathX(path);
        y = a0b30_env.getPathY(path);
    } else { *outY = (uint32_t)t[dir + 1]; *outX = (uint32_t)t[dir]; return; }
    *outX = (uint32_t)t[dir] + x;
    *outY = (uint32_t)t[dir + 1] + y;
}
