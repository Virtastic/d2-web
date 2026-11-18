// 1.13c D2Win+0x13770 -- release a control record: if its type word (+0) is 1, set +0x54 to -1
// and +0x58 to 0. A null pointer (string 0x1A4) or any other type (string 0x1A6) is fatal:
// the localized message goes through the Fog error ordinal 10024, then _exit(-1). The message
// dialog is a seam; we terminate through d2win_exit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void d2win_exit(uint32_t code);

extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_13770(void* p) {
    uint32_t type = 0;
    if (p) std::memcpy(&type, p, 4);
    if (p && type == 1) {
        const int32_t m1 = -1; const int32_t z = 0;
        std::memcpy((uint8_t*)p + 0x54, &m1, 4);
        std::memcpy((uint8_t*)p + 0x58, &z, 4);
        return;
    }
    d2win_exit((uint32_t)-1);
}
