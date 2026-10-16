// Shared by the d2mcpclient FN probes of the MCP request composers (6F20 6FB0 7020 70B0 7140), the retail capture tool/fn/d2mcpclient-<rva>.txt.
// Retail composes the request, sets [0xb1c8]=0 / [0xaeb4], then send(socket [0xb028], buf, len) through the Fog thunk 0x5DE6 and returns
// its result. Ours composes; this is the glue the vector records around it: the send callback logs buf+len (`w` log, lg), the hook log (hl)
// and the two gdump'd globals (gd). The blobs are not arguments: the replay mallocs gp, gd, lg, hl, then the string blobs in table
// order, so from the first string blob: hl = s-88, lg = hl-1128, gd = lg-40, gp = gd-40 (chunk = size+8 rounded to 8; emmalloc).
#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>
static uint32_t mcp_send(const void* s, const uint8_t* buf, uint32_t n, uint32_t aeb4) {
    uint32_t* hl = (uint32_t*)((const char*)s - 88), *lg = (uint32_t*)((const char*)hl - 1128), *gd = (uint32_t*)((const char*)lg - 40), *gp = (uint32_t*)((const char*)gd - 40);
    if (gp[0] != 0xC0DEAD01 || gp[1] != 0xC0DEAD02 || gp[2] != 0xC0DEAD03) abort();   // blob layout moved: fail loudly
    const uint32_t sock = gp[0];
    gd[0] = 0; gd[1] = aeb4 == ~0u ? gp[2] : (gp[2] & 0xFFFF0000u) | aeb4;   // [0xb1c8] = 0; [0xaeb4] low word := word (or untouched)
    lg[0] = 1; lg[1] = n; memcpy(lg + 2, buf, n); memset((char*)(lg + 2) + n, 0, (4 - n % 4) % 4);
    const uint32_t e[10] = { 0x5DE6, 0, 0, 0, 0, 0, sock, 0xB3000000u, n, 0 };   // the stack buffer is a local: 0xB3000000
    hl[0] = 1; memcpy(hl + 1, e, sizeof e);
    return 0xC0DE0010;   // the hook's scripted send result
}
