// Mapping lane: D2Client+0xBF6C0, build "DATA\GLOBAL\UI\<name>" and hand it to the loader at 0x2B420.
// Register arg: esi = name (NUL-terminated). Seam: 0xBE90 = Storm#506 strlen (ecx=str), 0x2B420 (eax=path, s0=0).
#include <cstdint>
#include <cstdio>
#include <emscripten.h>
#include "client_host.h"
extern "C" {
// 1.13c D2Client+0xBF6C0
EMSCRIPTEN_KEEPALIVE void d2c_bf6c0(D2cHost h, const char* name) {
    char buf[260];
    snprintf(buf, sizeof buf, "%s\\UI\\", "DATA\\GLOBAL");  // wsprintfA
    D2cCall c = {}; c.rva = 0xBE90; c.ecx = (uint32_t)(uintptr_t)buf;
    uint32_t n = h(&c);
    char* p = buf + n;
    while ((*p++ = *name++) != 0) {}
    D2cCall d = {}; d.rva = 0x2B420; d.eax = (uint32_t)(uintptr_t)buf; d.s[0] = 0;
    h(&d);
}
}
