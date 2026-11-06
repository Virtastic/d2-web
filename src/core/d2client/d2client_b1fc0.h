// Seams of D2Client 0xB1FC0 (client missile movement, table slot 34). Callees are named by their retail address.
#pragma once
#include <cstdint>
typedef uintptr_t U;
struct D2ClientB1fc0Env {
    uint32_t (*ld32)(U base, uint32_t off);
    U        (*ldp)(U base, uint32_t off);
    void     (*st32)(U base, uint32_t off, uint32_t v);
    U tbl;                                              // the unit hash table at D2Client+0x10A008 (128 chain heads)
    uint32_t (*f10818)(U m);                            // thunk 0xC4BA
    uint32_t (*f11018)(U u);                            // thunk 0xC322 (unit id)
    uint32_t (*f10522)(U m, U o);                       // thunk 0xC190 (distance)
    void     (*f10019)(U m, uint32_t s);                // thunk 0xC394
    void     (*f10334)(U ow, U m, uint32_t z);          // thunk 0xC376
    void     (*f10286)(U ow, uint32_t x, uint32_t y);   // thunk 0xC3EE
    void     (*f10488)(U ow, uint32_t a, uint32_t line);// thunk 0xC328 (file "Missile.cpp" at D2Client+0xD4010)
    uint32_t (*f1330)(U seed, uint32_t range);          // 0x1330: ECX = &seed, EAX = range
    uint32_t (*f1630)(U u);                             // 0x1630 (ECX = unit): x
    uint32_t (*f1660)(U u);                             // 0x1660 (ECX = unit): y
    void     (*f77bf0)(U m);                            // 0x77BF0 (EAX = unit)
    void     (*tail)(U m);                              // 0xB17C0 (ECX = unit), a tail jump
    void     (*die)();                                  // 10265(0x5a), 10024(str, r), _exit(-1); falls through if the host returns
};
// 1.13c D2Client+0xB1FC0: ECX = missile unit.
void d2c_b1fc0(const D2ClientB1fc0Env& e, U m);
