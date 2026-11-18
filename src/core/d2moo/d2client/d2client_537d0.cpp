// 1.13c D2Client+0x537D0 -- palshift cleanup (no arguments). g[0] = head of the palette-record list (module word 0x119608; the next link is the
// dword at record+0x1004), g[1] = slot table (0x117804), g[2] = remap table (0x11C2D8). Every record is released through Fog #10043
// (0xBF86: ECX = block, EDX = the source-file string D2Client+0xD5E7C, stack line, 0), the list head following each release; then the slot table
// (line 0x8A) and the remap table (line 0x90) are released and cleared. The head is read from its module word on every pass (it holds the next link).
#include "cl_env.h"
extern "C" void d2c_537d0r(uint32_t* g, const cl::Env& e) {
    constexpr uint32_t kFile = 0xB2000000u | 0xD5E7Cu;
    if (g[0]) {
        uint32_t cur = g[0], next;
        do {
            next = e.R(cur + 0x1004);
            e.H(0xbf86, 0x84, 0, 0, 0, 0, 0, cur, kFile);
            g[0] = next; cur = next;
        } while (cur);
        g[0] = 0;
    }
    if (g[1]) { e.H(0xbf86, 0x8a, 0, 0, 0, 0, 0, g[1], kFile); g[1] = 0; }
    if (g[2]) { e.H(0xbf86, 0x90, 0, 0, 0, 0, 0, g[2], kFile); g[2] = 0; }
}
