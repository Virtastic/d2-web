// 1.13c D2Client+0x64B30 -- ECX = menu item. Storm ordinal 423 loads the saved value (the key at 0xD3588, the name at 0xD51D0, 0, the item's value slot +0x124). When D2Gfx ordinal 10036 answers 0 the value is cleared and 0 returned. Otherwise the value is saved back through Storm ordinal 426, handed to D2Gfx ordinal 10081, and 0x8A4D0 runs (EAX = the dword 0xF9E18, ECX = the dword 0xF9E14, EDX = 0, ESI = the dword 0x11C3C4, one zero stack word); its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_f9e18;
__attribute__((weak)) uint32_t g_d2client_f9e14;
__attribute__((weak)) uint32_t g_d2client_11c3c4;
namespace d2cl {
uint32_t f64b30(GE& e, uint32_t it)
{
    const uint32_t slot = it + 0x124;
    e.h(0xBE8A, 0, 0, 0, 0, 0, 0xB20D3588u, 0xB20D51D0u, 0, slot);
    if (!e.h(0xD11A)) {
        e.wr32(slot, 0);
        return 0;
    }
    const uint32_t v = e.rd32(slot);
    e.h(0xBEF6, 0, 0, 0, 0, 0, 0xB20D3588u, 0xB20D51D0u, 0, v);
    e.h(0xD156, 0, 0, 0, 0, 0, v);
    return e.h(0x8A4D0, g_d2client_f9e18, g_d2client_f9e14, 0, 0, g_d2client_11c3c4, 0);
}
}
