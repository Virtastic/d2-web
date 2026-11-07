// 1.13c Storm+0x2560
// 1.13c Storm+0x2560: reuse-table init in the guest's 32-bit word layout: base init 0x2AF0, vtable 0x6FC36AD4, list {4} at word 10, list {0x10} at word 14, word 13 = 0x10.
#include <cstdint>
extern "C" uint32_t* Storm_Ctor2af0w(uint32_t* w);
extern "C" void Storm_Sub2BC0(uint32_t* n);
extern "C" void Storm_Sub2D20(uint32_t* p);
extern "C" uint32_t* Storm_Ctor2560w(uint32_t* w) {
    Storm_Ctor2af0w(w);
    w[0] = 0x6fc36ad4u;
    Storm_Sub2BC0(w + 10);
    Storm_Sub2D20(w + 14);
    w[13] = 0x10;
    if (w[10] != 4) { w[11] = (uint32_t)(uintptr_t)&w[11]; w[10] = 4; w[12] = ~(uint32_t)(uintptr_t)&w[11]; }
    return w;
}
