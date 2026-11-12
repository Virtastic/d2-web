// 1.13c Fog+0xD820: uint __stdcall(self, key), ret 8 -- bucket = self[3] & key; under the bucket's lock (self[7] + bucket * 0x18; the lock helper 0x11690
// is called first, ECX = lock, EDX = 0x76A) walk the chain self[5][bucket] (key +0, next +4) for node[0] == key. Absent returns 0. Found: take node[4] (the
// socket), leave the lock, then Ordinal_5 (thunk 0xCDB8, stdcall) with (socket, &16-byte buffer, &len = 0x10) and return the buffer's second dword.
#include "fog_env.h"
extern "C" uint32_t fog_d820(const FogEnv* e, uint32_t self, uint32_t key) {
    uint32_t bucket = fe_r32(self + 0xc) & key;
    uint32_t cs = fe_r32(self + 0x1c) + bucket * 0x18;
    fe_call(e, 0x11690, 0, cs, 0x76a);
    fe_enter(e, cs);
    uint32_t n = fe_r32(fe_r32(self + 0x14) + bucket * 4);
    while (n) { if (fe_r32(n) == key) break; n = fe_r32(n + 4); }
    if (!n) { fe_leave(e, cs); return 0; }
    uint32_t sock = fe_r32(n + 0x10);
    fe_leave(e, cs);
    uint32_t buf[4] = {0, 0, 0, 0}, len = 0x10;
    fe_call(e, 0xCDB8, 0, 0, 0, 0, 0, sock, (uint32_t)(uintptr_t)buf, (uint32_t)(uintptr_t)&len);
    return buf[1];   // mov eax,[esp+0x14]: the sockaddr's second dword (sin_addr), not the first

}
