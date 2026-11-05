// 1.13c D2Client+0x81720: ESI = arg. Calls thunk 0xBFE0 twice with ECX = [0x11BF60]
// then [0x11BF64]; when ESI != 0 tail-jumps 0x814F0 with ECX = ESI. The thunk and 0x814F0 are injected.
#include <cstdint>

struct D2c81720Env {
    void (*thunkBfe0)(uint32_t ecx);   // jmp [IAT] (Ordinal 10063), fastcall ECX
    void (*f814f0)(uint32_t ecx);      // 0x814F0, ECX = ESI
};

extern "C" void d2c_port_81720(uint32_t esi, uint32_t g11bf60, uint32_t g11bf64, const D2c81720Env& e) {
    e.thunkBfe0(g11bf60);
    e.thunkBfe0(g11bf64);
    if (esi) e.f814f0(esi);
}
