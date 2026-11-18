#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x220A0 (EAX = object): returns the dword at +0x7C. A null object is the fatal path
// (D2Lang string 0xCBB reported, then _exit(-1)); `fatal` stands for it.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_220a0_fn(const uint8_t* object, void (*fatal)(uint32_t)) {
    if (!object) { fatal(0xCBB); return 0; }
    uint32_t v;
    std::memcpy(&v, object + 0x7C, 4);
    return v;
}
