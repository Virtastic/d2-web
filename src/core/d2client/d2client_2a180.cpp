#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x2A180 (ESI = unit): clears bit 6 of the dword at unit+0xC4 when the unit is set, then calls
// Ordinal_11090 (0xC43C thunk) and, when it returns non-zero, 0x6A220. Callees are passed in; `ordArg` is the
// argument the retail thunk receives.
struct D2c2A180Env { uint32_t (*fb43c)(uint32_t, uint32_t); uint32_t (*f6a220)(uint32_t, uint32_t); };
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2a180(uint8_t* unit, uint32_t arg, uint32_t ordArg, const D2c2A180Env* e) {
    if (unit) {
        uint32_t f;
        std::memcpy(&f, unit + 0xC4, 4);
        f &= 0xFFFFFFBFu;
        std::memcpy(unit + 0xC4, &f, 4);
    }
    uint32_t u = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(unit));
    (void)ordArg;
    if (e->fb43c(u, arg) != 0) return e->f6a220(ordArg, arg);
    return 0;
}
