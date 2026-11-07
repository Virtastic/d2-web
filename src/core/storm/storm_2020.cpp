// 1.13c Storm+0x2020
// 1.13c Storm+0x2020: TSHashTable<RGN>-reuse ctor in the guest's 32-bit word layout (this = word array). It runs 0x2AF0 (base ctor:
// list {0xC}, 4-element list array, each element link offset 4), then the 0x2560 steps (list {4} at word 10, list {0x10} at word 14,
// word 13 = 0x10), the vtable, word 0x12 = ~this & 0x0FFFFFFF, word 0x13 = 0.
#include <cstdint>

extern "C" void Storm_Sub2BC0(uint32_t* n);
extern "C" void Storm_Sub2D20(uint32_t* p);
extern "C" void* Storm_FnArenaRealloc(void* p, uintptr_t size);

namespace {
inline uint32_t G(const void* p) { return (uint32_t)(uintptr_t)p; }
// the "if (list.linkoffset != off) { init }" step that follows each 0x2BC0 sentinel reset
void listInit(uint32_t* l, uint32_t off) {
    if (l[0] != off) { l[1] = G(&l[1]); l[0] = off; l[2] = ~G(&l[1]); }
}
}

extern "C" uint32_t* Storm_Ctor2020(uint32_t* w) {
    // 0x2AF0
    w[0] = 0x6fc36b04u;
    Storm_Sub2BC0(w + 1);
    w[5] = w[6] = w[7] = w[8] = 0; w[4] = 0;
    listInit(w + 1, 0xc);
    w[9] = 3;
    // 0x32E0 with wanted 4: capacity 4 (0x33C0 leaves the step), 0x3400 reallocates the (empty) array, then each element is reset by 0x3370
    w[5] = 4;
    w[7] = G(Storm_FnArenaRealloc((void*)(uintptr_t)w[7], 4 * 0xc));
    for (uint32_t i = 0; i < 4; i++) Storm_Sub2BC0((uint32_t*)(uintptr_t)w[7] + i * 3);
    w[6] = 4;
    for (uint32_t i = 0; i <= w[9]; i++) listInit((uint32_t*)(uintptr_t)w[7] + i * 3, 4);
    // 0x2560
    w[0] = 0x6fc36ad4u;
    Storm_Sub2BC0(w + 10);
    Storm_Sub2D20(w + 14);
    w[13] = 0x10;
    listInit(w + 10, 4);
    // 0x2020
    w[0] = 0x6fc36ae4u;
    w[0x12] = ~G(w) & 0x0fffffffu;
    w[0x13] = 0;
    return w;
}
