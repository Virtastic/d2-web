// 1.13c Storm+0x51F0: constructor of a Storm object. Zeroes words 0,1; runs the
// element ctor (Storm+0x5370) over three arrays of 4 x 12-byte elements at words 0x4c, 0x58, 0x64; then makes
// word 0x71 a self-linked node (w[0x71]=&w[0x71], w[0x72]=~&w[0x71], w[0x70]=0). Element ctor is a seam.
#include <cstdint>
#include <initializer_list>

struct StormObjEnv {
    void (*elem_ctor)(uint8_t* elem);  // Storm+0x5370 (dtor Storm+0x53C0 only used for SEH unwind)
};

extern "C" uint32_t* storm_sub_51f0(uint32_t* p, const StormObjEnv* e) {
    p[0] = 0;
    p[1] = 0;
    for (uint32_t base : {0x4cu, 0x58u, 0x64u})
        for (uint32_t i = 0; i < 4; i++) e->elem_ctor(reinterpret_cast<uint8_t*>(p + base) + i * 12);
    uint32_t* n = p + 0x71;
    n[0] = 0;  n[1] = 0;
    // retail stores the 32-bit address; wasm64 truncates to the low 32 bits
    n[0] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(n));
    p[0x70] = 0;
    n[1] = ~static_cast<uint32_t>(reinterpret_cast<uintptr_t>(n));
    return p;
}
