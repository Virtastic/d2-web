// 1.13c D2Win+0x08350 (sub_8350): on textbox ctrl (type 4) line node `index` (walk +0x48 head, next at +0x1c),
// free and null the five pointer slots node[0..4]. Any assert failure -> Fog #10265/#10024 -> _exit(-1).
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2Win8350Env {
    void (*free_mem)(int32_t line, int32_t zero);      // Fog #10043
};

// 1.13c D2Win+0x08350
void d2win_sub_8350(uint8_t* ctrl, int32_t index, D2Win8350Env& env) {
    auto rd = [](const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    if (!ctrl || rd(ctrl) != 4 || index > (int32_t)rd(ctrl + 0x54)) _exit(-1);
    uint8_t* node = (uint8_t*)(uintptr_t)rd(ctrl + 0x48);
    int32_t i = 0;
    for (; i < index && node; ) {
        uint8_t* nx = (uint8_t*)(uintptr_t)rd(node + 0x1c);
        if (!nx) break;
        i++; node = nx;
    }
    if (!node || i != index) _exit(-1);
    for (int k = 0; k < 5; k++)
        if (rd(node + k * 4)) { env.free_mem(0x231, 0); uint32_t z = 0; std::memcpy(node + k * 4, &z, 4); }
}
