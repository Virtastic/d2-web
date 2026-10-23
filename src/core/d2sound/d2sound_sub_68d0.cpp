// 1.13c D2sound+0x068D0 -- thiscall: if (this[0x34]&2) call vtbl slot 0x4c of the COM object at this[0xc] with (a,b,c,0).
#include <cstdint>
struct D2soundObj68D0 { uint32_t w[0x0d]; void* com; uint32_t flags; };
void D2sound_sub_68D0(const uint32_t* self, void (*callVt4c)(void* com, uint32_t, uint32_t, uint32_t, uint32_t),
                      uint32_t a, uint32_t b, uint32_t c, void (*fatal)(int)) {
    if (!self || !self[0]) { fatal(0x302); return; }
    if (self[0x34 / 4] & 2) callVt4c((void*)(uintptr_t)self[0xc / 4], a, b, c, 0);
}
