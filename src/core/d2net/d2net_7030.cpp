// 1.13c D2Net+0x7030 -- thread start (stdcall ret 4, the argument is the host name word): clears [B41C], starts the socket layer (0x5E0A with 0x101 and the data block at B260),
// opens a stream socket (0x5DE0 with 2, 1, 0) into [B248] (a -1 result also calls 0x5DF8), zeroes the address block B24C..B258, sets its family word to 2 and its port word (B24E) to
// the answer of 0x5DF2 for 0xFA0, resolves the argument through 0x5E2E (ECX) and 0x5E04, stores that in B250 and connects [B248] to the block (0x5E16, length 16); a -1 connect clears [B248].
// Returns 1. g = the five dwords B248..B258, t = [B41C].
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2n {
uint32_t f7030(GE& e, uint32_t param, uint32_t* g, uint32_t* t)
{
    *t = 0;
    e.h(0x5E0A, 0, 0, 0, 0, 0, 0x101, 0xB2000000u | 0xB260);
    g[0] = e.h(0x5DE0, 0, 0, 0, 0, 0, 2, 1, 0);
    if (g[0] == 0xFFFFFFFFu) e.h(0x5DF8);
    g[1] = g[2] = g[3] = g[4] = 0;
    g[1] = (g[1] & 0xFFFF0000u) | 2;
    const uint32_t port = e.h(0x5DF2, 0, 0, 0, 0, 0, 0xFA0) & 0xFFFF;
    g[1] = (g[1] & 0xFFFFu) | (port << 16);
    const uint32_t host = e.h(0x5E2E, 0, param);
    const uint32_t addr = e.h(0x5E04, 0, 0, 0, 0, 0, host);
    const uint32_t sock = g[0];
    g[2] = addr;
    if (e.h(0x5E16, 0, 0, 0, 0, 0, sock, 0xB2000000u | 0xB24C, 0x10) == 0xFFFFFFFFu) g[0] = 0;
    return 1;
}
}
