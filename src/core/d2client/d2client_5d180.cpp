// 1.13c D2Client+0x5D180 (56 B, no arguments, plain ret): if the slot module is initialised (DAT 0x11C280), release it
// through 0x5D110, free each of the 16 slot handles (the first dword of each 0x20-byte record at 0x1175C0) with Ordinal
// 10049 (thunk 0xD31E, ECX = handle) and zero that dword, then clear the flag.
#include <emscripten.h>
#include <cstdint>
struct D2c5d180Env {
    void (*c5d110)();
    void (*cd31e)(uint32_t ecx);
};
extern "C" void d2c_port_5d180(uint8_t* slots, uint32_t* g11c280, const D2c5d180Env* e) {
    if (*g11c280 == 0) return;
    e->c5d110();
    for (uint32_t* p = (uint32_t*)slots; p < (uint32_t*)(slots + 0x200); p += 8) {
        e->cd31e(*p);
        *p = 0;
    }
    *g11c280 = 0;
}
