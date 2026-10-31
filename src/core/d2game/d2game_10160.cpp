// 1.13c D2Game+0x10160: detach a unit.
// Convention: EAX = the value stamped at +0xC, ESI = pUnit, EDI = the game/owner word passed on. Callees are seams in
// retail argument order (the three D2Common imports are stdcall; 0x35890 takes EAX = unit, EBX = flag, [esp+4] = EDI).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game10160Env {
    void    (*imp_a65e)(void* unit, uint32_t a, uint32_t b);                 // import thunk 0xA65E (unit, 0x10, 1)
    void    (*imp_adae)(void* unit, uint32_t a, uint32_t b, uint32_t c);     // import thunk 0xADAE (unit, 0, 0, 0)
    void*   (*imp_a53e)(uint32_t unitId);                                    // import thunk 0xA53E (unitId or -1) -> record
    void    (*sub35890)(void* unit, uint32_t ebx, uint32_t edi);             // 0x35890: EAX = unit, EBX = flag-1, stack = EDI
    void    (*sub8E880)(uint32_t edi, void* unit);                           // 0x8E880: EAX = EDI, stack = unit
};
extern "C" void d2g_10160(void* unit, uint32_t eax, uint32_t edi, const D2Game10160Env* env) {  // 1.13c D2Game+0x10160
    uint8_t* u = (uint8_t*)unit;
    if (u) {
        uint32_t f; std::memcpy(&f, u + 0xC4, 4); f = (f & 0xFFFFFFF7u) | 2u;
        std::memcpy(u + 0xC, &eax, 4); std::memcpy(u + 0xC4, &f, 4);
    }
    env->imp_a65e(unit, 0x10, 1);
    env->imp_adae(unit, 0, 0, 0);
    uint32_t id = 0xFFFFFFFFu; if (u) std::memcpy(&id, u + 4, 4);
    const uint8_t* rec = (const uint8_t*)env->imp_a53e(id);
    uint8_t flag = rec[0x12A];
    if (flag) env->sub35890(unit, (uint32_t)flag - 1u, edi);
    if (u) {
        uint32_t p; std::memcpy(&p, u + 0xDC, 4);
        if (p) env->sub8E880(edi, unit);
        uint32_t z = 0; std::memcpy(u + 0xDC, &z, 4);
    }
}
