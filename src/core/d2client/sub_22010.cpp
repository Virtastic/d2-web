#include <stdint.h>
// 1.13c D2Client+0x22010
extern "C" void d2_client_22010(uint8_t* obj, uint32_t v) {
    if (!obj) return; // retail: report error + _exit(-1)
    *(uint32_t*)(obj + 0x80) = v;
}
