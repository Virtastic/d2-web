// 1.13c Fog+0xDF00: void __stdcall(key, a, b), ret 0xC, ESI = srv -- register a pending key. srv[0xBD0] == 0 returns. 0xCEB0(EAX = srv, stack key) nonzero
// returns. The callback at srv+0xBB8, when set, is called (ECX = key, EDX = a, stack b). Under the +0x894 lock (helper 0x11690, EDX 0x492): bucket =
// key % srv[0x8B0] in the table srv[0x8AC]; a node with node[0] == key gets node[2] = 0x78 and the function returns. Otherwise a node comes from the free
// list srv[0x88C] (next +0xC) or 0x1CD10 (ECX 0x10, EDX file 0x6FF78FAC, stack line 0x4A6, 0); node = {key, 0x78, 0, old bucket head}, the bucket head is the
// node, srv[0x890]++, the lock is left and 0xD230(stack count) runs.
// Returns the EAX retail leaves: 0 (the +0xBD0 test), the 0xCEB0 result, LeaveCriticalSection's 0 (the hit), or the 0xD230 result.
#include "fog_env.h"
extern "C" uint32_t fog_df00(const FogEnv* e, uint32_t esi, uint32_t key, uint32_t a, uint32_t b) {
    if (fe_r32(esi + 0xbd0) == 0) return 0;
    if (uint32_t r = fe_call(e, 0xCEB0, esi, 0, 0, 0, 0, key)) return r;
    uint32_t cb = fe_r32(esi + 0xbb8);
    if (cb) fe_call(e, cb & 0xFFFFFF, 0, key, a, 0, 0, b);
    uint32_t cs = esi + 0x894;
    fe_call(e, 0x11690, 0, cs, 0x492);
    fe_enter(e, cs);
    uint32_t bucket = key % fe_r32(esi + 0x8b0);
    for (uint32_t n = fe_r32(fe_r32(esi + 0x8ac) + bucket * 4); n; n = fe_r32(n + 0xc))
        if (fe_r32(n) == key) { fe_w32(n + 8, 0x78); fe_leave(e, cs); return 0; }
    uint32_t n = fe_r32(esi + 0x88c);
    if (n == 0) n = fe_call(e, 0x1CD10, 0, 0x10, 0x6FF78FAC, 0, 0, 0x4a6, 0);
    else fe_w32(esi + 0x88c, fe_r32(n + 0xc));
    fe_w32(n, key); fe_w32(n + 4, 0x78); fe_w32(n + 8, 0);
    fe_w32(n + 0xc, fe_r32(fe_r32(esi + 0x8ac) + bucket * 4));
    fe_w32(fe_r32(esi + 0x8ac) + bucket * 4, n);
    uint32_t cnt = fe_r32(esi + 0x890) + 1;
    fe_w32(esi + 0x890, cnt);
    fe_leave(e, cs);
    return fe_call(e, 0xD230, 0, 0, 0, 0, 0, fe_r32(esi + 0x890));
}
