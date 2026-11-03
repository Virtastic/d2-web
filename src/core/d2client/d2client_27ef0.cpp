// 1.13c D2Client+0x27EF0: bottom-centre button hint text. Every callee goes through the injected dispatcher;
// the Unicode imports are keyed by their IAT slot RVA (0xCEB00 ctor, 0xCEAB4 sprintf, 0xCEAB8 strcpy, 0xCEABC strcat).
#include <cstring>
#include "d2client_callee.h"
// 1.13c D2Client+0x27EF0
extern "C" void d2c_27ef0(const D2cSvc& v, int32_t w /*0xDBC48*/, int32_t h /*0xDBC4C*/, const uint8_t* tbl /*0x11A2E0: 114 entries of 10 bytes {id:4, key:2, state:4}*/) {
    uint16_t big[200], small[30];
    for (int i = 0; i < 200; ++i) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&big[i], 0, 0, 0);     // Unicode ctor
    for (int i = 0; i < 30; ++i) d2c_call(v, 0xceb00, 0, (uint32_t)(uintptr_t)&small[i], 0, 0, 0);
    const uint32_t base = d2c_call(v, 0xd1ec, 0, 0x1053, 0, 0, 0);
    d2c_call(v, 0xceab8, 0, (uint32_t)(uintptr_t)big, base, 0, 0);                                      // Unicode::strcpy
    for (uint32_t state = 1; state != 0xffffffffu; state = state ? 0 : 0xffffffffu) {
        int idx = -1;
        for (int i = 0; i < 114; ++i) {
            uint32_t id, st; std::memcpy(&id, tbl + i * 10, 4); std::memcpy(&st, tbl + i * 10 + 6, 4);
            if (id == 0x23 && st == state) { idx = i; break; }
        }
        if (idx < 0) continue;
        uint16_t key; std::memcpy(&key, tbl + idx * 10 + 4, 2);
        if (key == 0xffff) continue;
        std::memset(small, 0, sizeof small);
        const uint32_t s = d2c_call(v, 0x39f30, 0x23, 0, state, 0, 0);
        const uint32_t fmt = d2c_call(v, 0xd1ec, 0, 0x1052, 0, 0, 0, s);
        d2c_call(v, 0xceab4, 0, 0, 0, 0, 0, 0x1e, (uint32_t)(uintptr_t)small, fmt, s);                  // Unicode::sprintf
        d2c_call(v, 0xceabc, 0, (uint32_t)(uintptr_t)big, (uint32_t)(uintptr_t)small, 0, 0);            // Unicode::strcat
    }
    d2c_call(v, 0xd384, 0, (uint32_t)(uintptr_t)big, (uint32_t)(((w - (w >> 31)) >> 1) - 0x91), 0, 0, (uint32_t)(h - 0x17), 0, 1);
}
