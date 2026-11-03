// D2Client+0x60AA0: the status line under the world draw. Interface-shaped; globals and callees are passed in (decompile-derived).
#include <cstdint>
struct D2cStatusLineState { uint32_t bbfc; uint32_t c21c; uint32_t e14; };   // 11BBFC, 11C21C, F9E14
struct D2cStatusLineEnv {
    uint32_t (*ord10331)(uint32_t);                                  // 0xC1F0: lookup of [11BBFC]
    uint32_t (*ord10826)(uint32_t);                                  // 0xC1DE (result unused by retail)
    uint32_t (*fBE240)();                                            // 0xBE240, returns the object used below
    uint32_t (*ord10028)(uint32_t ecx, uint32_t a, uint32_t b, uint32_t c);   // 0xD35A, ECX = the object, stack (c21c, 4, 0)
    void (*ord10150)(uint32_t ecx, uint32_t edx);                    // 0xD37E, ECX = the object, EDX
};
// 1.13c D2Client+0x60AA0
void d2c_60aa0(D2cStatusLineState* s, const D2cStatusLineEnv* e) {
    if (s->bbfc == 0) return;
    uint32_t looked = e->ord10331(s->bbfc);
    if (looked == 0) return;
    e->ord10826(looked);
    uint32_t obj = e->fBE240();
    uint32_t w = e->ord10028(obj, s->c21c, 4, 0);
    e->ord10150(obj, s->e14 - w - 0x10);
    s->c21c += 0x10;
}
