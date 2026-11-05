// 1.13c D2Client+0xA5310 (46 B, EAX = object or null, stack word = id, ret 4): looks the id up through
// ordinal 10093 with the object's +4 (or -1 for a null object); a hit returns ordinal 10014(hit) + 0x1BE, a miss 0.
#include <emscripten.h>
#include <cstdint>
extern "C" uint32_t d2c_port_a5310(uint32_t obj, uint32_t id, uint32_t (*o10093)(uint32_t, uint32_t), uint32_t (*o10014)(uint32_t)) {
    uint32_t k = obj ? *(const uint32_t*)(uintptr_t)(obj + 4) : 0xFFFFFFFFu;
    uint32_t r = o10093(id, k);
    return r ? o10014(r) + 0x1BEu : 0;
}
