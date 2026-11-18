// Free a unit's packet queue. Retail: EDI = unit (retail 32-bit layout), plain ret. A null unit is fatal (Fog 10265(0x8A5),
// Fog 10024, _exit(-1)). If the queue pointer at unit+0xD8 is set, the queue's buffer (queue+0xC) is freed at line 0x1515,
// then the queue itself at line 0x1516 (Fog 10043 through thunk 0xBF86: ECX = block, EDX = source file name, stack line, 0;
// ret 8), and unit+0xD8 is cleared.
// 1.13c D2Client+0xABD40
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cAbd40Env {
    void (*fatal)(uint32_t code);                    // Fog 10265 + Fog 10024 + _exit(-1)
    void (*free)(uint32_t block, uint32_t line);     // thunk 0xBF86 (Fog 10043), file D2Client+0xD4034, trailing 0
};

static uint32_t rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_abd40(uint8_t* unit, const D2cAbd40Env* env) {
    if (!unit) { env->fatal(0x8a5); return; }
    const uint32_t q = rd32(unit + 0xd8);
    if (!q) return;
    env->free(rd32((const uint8_t*)(uintptr_t)q + 0xc), 0x1515);
    env->free(q, 0x1516);
    const uint32_t z = 0;
    std::memcpy(unit + 0xd8, &z, 4);
}
