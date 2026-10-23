// src/core/d2win/d2win_sub_18560.cpp -- 1.13c D2Win+0x18560, from the cached Ghidra decompile.
// Control-create dispatcher: p[0] selects the constructor, p[1..] are its arguments.
#include <cstdint>

// Constructors take the decompiled argument lists; their hidden register/env args are bound by the
// caller (ponytail: callee table instead of direct calls, since each sibling has its own env shape).
struct D2WinCreateTbl {
    void* (*c1)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const uint32_t*);  // 161B0
    void* (*c2)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);                                       // 103D0
    void* (*c3)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);                   // 10C50
    void* (*c4)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const uint32_t*, const uint32_t*);     // 9C00
    void* (*c5)(uint32_t, uint32_t, uint32_t, uint32_t);                                                           // B160
    void* (*c6)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);         // FD70
    void* (*c7)(uint32_t, uint32_t, const uint32_t*, const uint32_t*);                                             // 16DD0
    void* (*c8)();                                                                                                 // 164D0
    void* (*c9)(uint32_t, uint32_t, uint32_t);                                                                     // 115A0
    void* (*c10)(uint32_t, uint32_t);                                                                              // EC70
    void* (*c12)(uint32_t, uint32_t, uint32_t, const uint32_t*);                                                   // A250
    void* (*c13)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);                                                // 10610
};

// 1.13c D2Win+0x18560
// p is a 32-bit-word request block (retail layout); p[7] is an optional pointer whose first dword is
// forwarded (0 if null). Unknown type -> null.
extern "C" void* d2win_sub_18560(const uint32_t* p, const D2WinCreateTbl& t) {
    uint32_t u = p[7] ? *(const uint32_t*)(intptr_t)p[7] : 0;  // ponytail: wasm64 addresses must fit 32 bits
    auto P = [&](int i) { return (const uint32_t*)(intptr_t)p[i]; };
    switch (p[0]) {
    case 1:  return t.c1(p[1], p[2], p[3], p[4], p[5], p[6], u, p[9], P(8));
    case 2:  return t.c2(p[1], p[2], p[3], p[4], u, p[8]);
    case 3:  return t.c3(p[1], p[2], p[3], p[4], u, p[5], p[8], p[9]);
    case 4:  return t.c4(p[1], p[3], p[4], p[5], p[6], u, P(8), P(0xb));
    case 5:  return t.c5(p[1], p[2], p[3], p[4]);
    case 6:  return t.c6(p[1], p[2], p[3], p[4], u, p[8], p[5], p[9], p[0xb]);
    case 7:  return t.c7(p[1], p[2], P(3), P(4));
    case 8:  return t.c8();
    case 9:  return t.c9(p[1], p[2], p[3]);
    case 10: return t.c10(p[1], p[2]);
    case 0xc: return t.c12(p[1], p[2], p[3], P(4));
    case 0xd: return t.c13(p[1], p[2], p[3], p[4], u);
    default: return nullptr;
    }
}
