// 1.13c D2Client port; spec in the private store.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Client+0x25B40: EAX/ESI = object. Call 0x25660 (EAX = ESI = object), then read the byte at (*(*(object+0x14)))+0x1B4: if it is
// <= 3, or else 0x23680() returns non-zero in AL, call 0x227E0(object) and return its EAX; otherwise return EAX as 0x23680 left it.
struct D2c25B40Env { void (*f25660)(uint32_t objTok); uint32_t (*f23680)(); uint32_t (*f227e0)(uint32_t objTok); };
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_25b40(const uint8_t* object, uint32_t objTok, const D2c25B40Env* e) {
    e->f25660(objTok);
    uint32_t p1; std::memcpy(&p1, object + 0x14, 4);
    uint32_t p2; std::memcpy(&p2, (const uint8_t*)(uintptr_t)p1, 4);
    uint32_t eax;
    if (((const uint8_t*)(uintptr_t)p2)[0x1B4] > 3) {
        eax = e->f23680();
        if ((eax & 0xFF) == 0) return eax;
    }
    return e->f227e0(objTok);
}
