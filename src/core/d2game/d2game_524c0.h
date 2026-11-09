// 1.13c D2Game+0x524C0 (the game task-queue merge): stdcall(pQueue), ret 4, no result.
// Written from the 1.13c disassembly as a rule over an environment E (rd32/wr32 over blob and module addresses; E::S is the module address
// of the worker-list sentinel 0x110398, E::g(rva) a module global). Rule: quot = g(0x1103AC) / g(0x1103A8), rem = the remainder,
// t = min(g(0x1103A8) - 1, 0x20) - rem. For every worker N on the sentinel chain (S[1], next at N[1]): t -= 1; take = N[-3] - 1 when
// (t >= 0) + quot is 0, else N[-3]; under the worker's critical section (0x111C58 + 0x18 * N[-4]) the worker sublist L = N-8 is cut after
// `take` steps from L[0] (stopping at L): a[1] := L, L[0] := a, rest = the old a[1]. A non-empty rest is merged into the queue's list P =
// pQueue+8, ordered by the item word [-3] (an item goes after every queue item whose key is greater, ties stay ahead), the old L[0] getting
// [1] := 0 first and closing the merge (old L[0] [1] := P, P[0] := old L[0]); a null [1] ends the rest early. Finally pQueue+0x10 is
// pushed at the chain front: x[0] := S, x[1] := S[1], S[1][0] := x, S[1] := x.
#pragma once
#include <cstdint>

namespace d2g524c0 {
template <class E> void Merge(E& e, uint32_t queue)
{
    const uint32_t S = E::S;
    const uint32_t cnt = e.g(0x1103A8), tot = e.g(0x1103AC);
    const uint32_t quot = tot / cnt, rem = tot % cnt;
    int32_t t = (int32_t)((cnt - 1 < 0x20 ? cnt - 1 : 0x20) - rem);
    for (uint32_t N = e.rd32(S + 4); N != S; N = e.rd32(N + 4)) {
        --t;
        uint32_t n = e.rd32(N - 0xC) - (((t >= 0 ? 1u : 0u) + quot) == 0 ? 1u : 0u);
        const uint32_t L = N - 8, L0 = e.rd32(L);
        uint32_t a = L0;
        while (n) { --n; if (a == L) break; a = e.rd32(a + 4); }
        uint32_t it = e.rd32(a + 4);
        e.wr32(a + 4, L);
        e.wr32(L, a);
        if (it == L) continue;
        const uint32_t P = queue + 8;
        e.wr32(L0 + 4, 0);
        uint32_t pos = e.rd32(P);
        if (pos != P) {
            const uint32_t key = e.rd32(it - 0xC);
            while ((int32_t)(key - e.rd32(pos - 0xC)) < 0) { pos = e.rd32(pos); if (pos == P) break; }
        }
        bool ended = false;
        while (e.rd32(pos + 4) != P) {
            const uint32_t nx = e.rd32(it + 4);
            e.wr32(it, pos);
            const uint32_t c = e.rd32(pos + 4);
            e.wr32(it + 4, c);
            e.wr32(c, it);
            e.wr32(pos + 4, it);
            if (!nx) { ended = true; break; }
            pos = it;
            uint32_t c2 = e.rd32(pos + 4);
            it = nx;
            if (c2 != P) {
                const uint32_t k = e.rd32(nx - 0xC);
                while ((int32_t)(k - e.rd32(c2 - 0xC)) > 0) { pos = c2; c2 = e.rd32(pos + 4); if (c2 == P) break; }
            }
        }
        if (ended) continue;
        e.wr32(it, pos);
        e.wr32(L0 + 4, P);
        e.wr32(pos + 4, it);
        e.wr32(P, L0);
    }
    const uint32_t x = queue + 0x10;
    e.wr32(x, S);
    e.wr32(x + 4, e.rd32(S + 4));
    e.wr32(e.rd32(S + 4), x);
    e.wr32(S + 4, x);
}
}
