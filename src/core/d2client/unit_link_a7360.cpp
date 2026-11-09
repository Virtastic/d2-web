// 1.13c D2Client+0xA7360 (127 B): EAX = unit (kept in ESI across the calls). 0xA6500 (ESI = unit) nonzero returns the unit. Else 0xA4E20 (EAX = id & 0x7F, ECX = id,
// EDX = table A + (type << 9), stack type; ret 4) looks the id up: a hit equal to the unit is the fatal path (0x1C6); a hit p relinks it through 0xA6EF0 (ECX = row, EDX = pid & 0x7F,
// stack pid then type; ret 8) with row = table B + (type << 9) when bit 21 of p[0xC4] is set, else table A; 0xA6500 again nonzero returns the unit. Otherwise 0xA6D60 (stdcall unit)
// frees the unit and the result is 0.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t R(uintptr_t a) { return *(const uint32_t*)a; }

extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t d2_client_a7360(uintptr_t unit, uintptr_t tab_a, uintptr_t tab_b, uint32_t (*f6500)(uintptr_t esi),
                                                          uintptr_t (*f4e20)(uint32_t eax, uint32_t ecx, uintptr_t edx, uint32_t type), void (*f6ef0)(uintptr_t ecx, uint32_t edx, uint32_t pid, uint32_t type),
                                                          void (*f6d60)(uintptr_t unit), void (*fatal)(uint32_t code)) {
    if (f6500(unit)) return unit;
    uint32_t type = R(unit), id = R(unit + 12);
    uintptr_t p = f4e20(id & 0x7f, id, tab_a + ((uintptr_t)type << 9), type);
    if (p == unit) fatal(0x1c6);
    if (p) {
        uint32_t t = R(p), pid = R(p + 12);
        uintptr_t row = ((R(p + 0xc4) >> 0x15) & 1) ? tab_b + ((uintptr_t)t << 9) : tab_a + ((uintptr_t)t << 9);
        f6ef0(row, pid & 0x7f, pid, t);
        if (f6500(unit)) return unit;
    }
    f6d60(unit);
    return 0;
}
