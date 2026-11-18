// 1.13c D2Client+0x7C9F0 (78 B, no arguments): when the current id word [0x11BF9C] is nonzero, walk the list [0x11BF30]
// (node = {entry*, next*}; entry +4 = id, +8 = object) for the first entry whose id equals it and, when its object is nonzero,
// call 0x88230 (EAX = 4, stack object, 0, 0); then clear the id word. The dword at 0x113B0B is always cleared.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" void d2c_port_7c9f0(uint32_t* curId, uint32_t head, uint8_t* flag4, void (*c88230)(uint32_t obj)) {
    if (*curId != 0) {
        for (uint32_t n = head; n != 0; n = *(const uint32_t*)(uintptr_t)(n + 4)) {
            uint32_t e = *(const uint32_t*)(uintptr_t)n;
            if (*(const uint32_t*)(uintptr_t)(e + 4) == *curId) {
                uint32_t obj = *(const uint32_t*)(uintptr_t)(e + 8);
                if (obj != 0) c88230(obj);
                break;
            }
        }
        *curId = 0;
    }
    uint32_t z = 0; memcpy(flag4, &z, 4);
}
