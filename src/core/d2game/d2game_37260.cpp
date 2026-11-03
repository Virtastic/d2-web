// D2Game+0x37260: EAX = flag, EBX = struct with a handle at +0x60, no stack arguments, plain ret (callee-saved esi/edi restored; esi is a saved
// register, not an input). EAX == 0 does nothing. Else a = Ordinal 10061(h) (thunk 0xA54A); a && Ordinal 11160(a, 0) (0xA952) calls Ordinal 10164(ebx, a, 1)
// (0xA8C2); b = Ordinal 10962(h) (0xB2AC); b && b != a && Ordinal 11160(b, 0) calls Ordinal 10164(ebx, b, 0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game37260Env {
    uint32_t (*first)(uint32_t h);                             // Ordinal 10061
    uint32_t (*test)(uint32_t v, uint32_t z);                  // Ordinal 11160
    void     (*apply)(uint32_t owner, uint32_t v, uint32_t f); // Ordinal 10164
    uint32_t (*second)(uint32_t h);                            // Ordinal 10962
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x37260
extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_37260(uint32_t flag, uint32_t ebx, const D2Game37260Env* e)
{
    if (!flag) return;
    const uint32_t a = e->first(rd32(ebx + 0x60));
    if (a && e->test(a, 0)) e->apply(ebx, a, 1);
    const uint32_t b = e->second(rd32(ebx + 0x60));
    if (b && b != a && e->test(b, 0)) e->apply(ebx, b, 0);
}
