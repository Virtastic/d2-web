// 1.13c D2Multi+0xB670: join-game gate. ctx = [39f14]; vtable [3917c] +0x68 (thunk 0x7bfc in the capture) and +0x44 (thunk 0x7c02: ECX = ctx+0x229,
// EDX = 0x6f9e79c0, returns a record: dword length, then bytes). gv: +0 [3a09c] (out).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_b670(const D2mEnv& e, uint32_t* gv, uint8_t* ctx) {
    if (ctx[0x229] == 0) return 1;
    if (d2m_call(e, 0x7bfc) == 0) return 0;
    if (d2m_call(e, 0x7e2a) != 0) return 1;
    gv[0] = 0;
    const uint32_t t = d2m_call(e, 0x7ea8, 0, 0x147b);
    d2m_call(e, 0xaa60, 0, 0, 0, D2M_MOD(0x90b0), 0, t);
    const uint32_t p = d2m_call(e, 0x7c02, 0, (uint32_t)(uintptr_t)(ctx + 0x229), D2M_MOD(0x179c0));
    if (p == 0) return 0;
    uint32_t len; __builtin_memcpy(&len, (const void*)(uintptr_t)p, 4);
    if (len <= 8) return 0;
    d2m_call(e, 0x7e42, 0, 0, 0, 0, 0, p + 4);
    d2m_call(e, 0x7cce, 0, 0, 0, 0, 0, D2M_MOD(0x8f60));
    d2m_call(e, 0x8790);
    if (gv[0] == 0 && d2m_call(e, 0x7e2a) != 0 && d2m_call(e, 0x7e1e) == 0) {
        d2m_call(e, 0x7e9c, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)(ctx + 0xb9));
        d2m_call(e, 0x7cce, 0, 0, 0, 0, 0, D2M_MOD(0x8f20));
        d2m_call(e, 0x8790);
        return d2m_call(e, 0x7e1e) == 0;
    }
    d2m_call(e, 0x8790);
    return 0;
}
