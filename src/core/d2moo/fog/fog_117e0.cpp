// 1.13c Fog+0x117E0 -- ECX = argument: the Fog teardown. Does nothing unless the init flag (0x56E78) is set; then runs the
// shutdown chain (0x1D7F0 with the caller's ECX, 0x1B830(0), clear the 0x56E6C flag, 0x144C0, 0x184A0), takes and deletes the
// lock at 0x316D8 (0x11690 registers it with id 0x4E), runs 0x18BB0, clears the init flag and deletes the locks at 0x32AEC and 0x32C0C.
// Lock calls are Win32 imports and have no effect in wasm; the callees are worker seams.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace fg {
void f117e0(GE& e, uint32_t ecx, uint32_t initFlag, uint32_t flag6c)
{
    if (e.rd32(initFlag) == 0) return;
    e.h(0x1D7F0, 0, ecx);
    e.h(0x1B830, 0, 0, 0, 0, 0, 0);
    e.wr32(flag6c, 0);
    e.h(0x144C0, 0);
    e.h(0x184A0, 0);
    e.h(0x11690, 0, 0xB20316D8u, 0x4E);
    e.h(0x18BB0, 0);
    e.wr32(initFlag, 0);
}
}
