// src/core/d2win/d2win_sub_18560.cpp -- 1.13c D2Win+0x18560: control-create dispatcher.
// stdcall (p), ret 4: p[0] (1..13) selects the constructor, p[1..] are its arguments; p[7] is an optional
// pointer whose first dword is forwarded (0 if null). Every argument list is the retail one, read off the
// disassembly: ECX, EDX, then the stack arguments in order (the callee's own ret size).
#include <cstdint>

using u32 = uint32_t;
struct D2WinCreateTbl {
    void* (*c1)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);   // 0x161B0 (ECX, EDX, 9 stack)
    void* (*c2)(u32, u32, u32, u32, u32, u32, u32, u32);                  // 0x103D0 (ECX, EDX, 6)
    void* (*c3)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);        // 0x10C50 (ECX, EDX, 8)
    void* (*c4)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);   // 0x9C00  (ECX, EDX, 9)
    void* (*c5)(u32, u32, u32, u32, u32, u32);                            // 0xB160  (ECX, EDX, 4)
    void* (*c6)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);   // 0xFD70  (ECX, EDX, 9)
    void* (*c7)(u32, u32, u32, u32, u32, u32);                            // 0x16DD0 (ECX, EDX, 4)
    void* (*c8)(u32, u32);                                                // 0x164D0 (2 stack)
    void* (*c9)(u32, u32, u32, u32, u32);                                 // 0x115A0 (ECX, EDX, 3)
    void* (*c10)(u32, u32, u32, u32);                                     // 0xEC70  (ECX, EDX, 2)
    void* (*c12)(u32, u32, u32, u32, u32, u32);                           // 0xA250  (ECX, EDX, 4)
    void* (*c13)(u32, u32, u32, u32, u32, u32, u32);                      // 0x10610 (7 stack)
};

// 1.13c D2Win+0x18560
extern "C" void* d2win_sub_18560(const u32* p, const D2WinCreateTbl& t) {
    u32 u = p[7] ? *(const u32*)(uintptr_t)p[7] : 0;
    switch (p[0]) {
    case 1:  return t.c1(p[1], p[2], p[3], p[4], p[5], p[6], u, p[9], p[8], p[11], p[10]);
    case 2:  return t.c2(p[1], p[2], p[3], p[4], u, p[8], p[9], p[10]);
    case 3:  return t.c3(p[1], p[2], p[3], p[4], u, p[5], p[8], p[9], p[6], p[10]);
    case 4:  return t.c4(p[1], p[2], p[3], p[4], p[5], p[6], u, p[8], p[11], p[10], p[9]);
    case 5:  return t.c5(p[1], p[2], p[3], p[4], u, p[8]);
    case 6:  return t.c6(p[1], p[2], p[3], p[4], u, p[8], p[5], p[9], p[11], p[6], p[10]);
    case 7:  return t.c7(p[1], p[2], p[3], p[4], p[9], p[10]);
    case 8:  return t.c8(p[1], p[8]);
    case 9:  return t.c9(p[1], p[2], p[3], p[4], p[8]);
    case 10: return t.c10(p[1], p[2], p[3], p[4]);
    case 12: return t.c12(p[1], p[2], p[3], p[4], p[11], p[10]);
    case 13: return t.c13(p[1], p[2], p[3], p[4], u, p[5], p[6]);
    default: return nullptr;   // 11 and anything outside 1..13
    }
}
