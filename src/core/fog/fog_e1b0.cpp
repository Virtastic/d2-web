// 1.13c Fog+0xE1B0: void __stdcall(key), ret 4, EAX = srv -- drop a QServer connection record by id. idx = key & srv[3]; under the lock srv[7] + idx * 0x18
// (helper 0x11690 before every Enter and Leave, EDX = the source line) the chain srv[5][idx] (id at +0, next at +4) is searched; no hit leaves and returns.
// A hit decrements srv[0x880] under srv+0x854, unlinks the node from the chain and clears node[1]. Retail then re-checks node[0] == key; a mismatch is the
// fatal path (0xFC20, Ordinal_116, 0x10F30, exit), unreachable single-threaded and not modelled. Then 0x1C9E0 (result unused), Ordinal_11 (thunk 0xCDA6) on
// node[6], and under srv+0x28 the chain srv[8][node[6] & srv[3]] (id at +0, next at +0x1C) is searched for the same key: a hit is unlinked, a miss runs
// Ordinal_11 again. Last, under srv+0x854: 0xD0A0 (EAX = &node[6], EDI = srv).
#include "fog_env.h"
static void lk(const FogEnv* e, uint32_t cs, uint32_t line) { fe_call(e, 0x11690, 0, cs, line); }
extern "C" void fog_e1b0(const FogEnv* e, uint32_t srv, uint32_t key) {
    uint32_t idx = key & fe_r32(srv + 0xC);
    uint32_t off = idx * 0x18;
    uint32_t cs = fe_r32(srv + 0x1C) + off;
    lk(e, cs, 0x3B5); fe_enter(e, cs);
    uint32_t prev = 0, n = fe_r32(fe_r32(srv + 0x14) + idx * 4);
    while (n && fe_r32(n) != key) { prev = n; n = fe_r32(n + 4); }
    if (!n) {
        cs = fe_r32(srv + 0x1C) + off;
        lk(e, cs, 0x3C7); fe_leave(e, cs);
        return;
    }
    uint32_t cs2 = srv + 0x854;
    lk(e, cs2, 0x3CD); fe_enter(e, cs2);
    fe_w32(srv + 0x880, fe_r32(srv + 0x880) - 1);
    lk(e, cs2, 0x3CF); fe_leave(e, cs2);
    uint32_t slot = fe_r32(srv + 0x14) + idx * 4;
    if (n == fe_r32(slot)) fe_w32(slot, fe_r32(n + 4)); else fe_w32(prev + 4, fe_r32(n + 4));
    fe_w32(n + 4, 0);
    cs = fe_r32(srv + 0x1C) + off;
    lk(e, cs, 0x3DA); fe_leave(e, cs);
    fe_call(e, 0x1C9E0);
    fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, fe_r32(n + 0x18));
    uint32_t cs3 = srv + 0x28;
    lk(e, cs3, 0x3E7); fe_enter(e, cs3);
    uint32_t slot2 = fe_r32(srv + 0x20) + (fe_r32(n + 0x18) & fe_r32(srv + 0xC)) * 4;
    uint32_t head = fe_r32(slot2), p2 = 0, m = head;
    while (m && fe_r32(m) != key) { p2 = m; m = fe_r32(m + 0x1C); }
    if (!m) fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, fe_r32(n + 0x18));
    else if (m == head) fe_w32(slot2, fe_r32(m + 0x1C));
    else fe_w32(p2 + 0x1C, fe_r32(m + 0x1C));
    lk(e, cs3, 0x3FB); fe_leave(e, cs3);
    lk(e, cs2, 0x3FE); fe_enter(e, cs2);
    fe_call(e, 0xD0A0, n + 0x18);
    lk(e, cs2, 0x406); fe_leave(e, cs2);
}
