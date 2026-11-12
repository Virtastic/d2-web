// 1.13c D2Multi+0xC490 -- cdecl, no arguments: queue an empty message node. A 16-byte node comes from the Fog allocator
// (thunk 0x7BE4: ECX = 0x10, EDX = the source file name at image offset 0x1798C, stack words line 0xC17 and 0), is
// cleared, gets kind 1 at +8, and is appended to the list whose head and tail are the globals at 0x3A0E8 / 0x3A0EC (the
// link is the dword at +0xC) inside the 0x38F00 critical section, a no-op on our single thread. Returns 1.
// Retail clears the node before it tests the pointer, so a failed allocation faults there too.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a0e8;
__attribute__((weak)) uint32_t g_d2multi_3a0ec;
namespace d2multi {
uint32_t fc490(GE& e)
{
    const uint32_t node = e.h(0x7BE4, 0, 0x10, 0xB2000000u | 0x1798C, 0, 0, 0xC17, 0);
    for (uint32_t o = 0; o < 16; o += 4) GE::wr32(node + o, 0);
    if (node) {
        GE::wr32(node + 8, 1);
        if (g_d2multi_3a0e8) GE::wr32(g_d2multi_3a0ec + 0xC, node);
        else g_d2multi_3a0e8 = node;
        g_d2multi_3a0ec = node;
    }
    return 1;
}
}
