// 1.13c D2Client+0xBB8F0 (587 B, written from the disassembly): ECX = client missile unit, plain ret. Row = Missiles row of unit[+4]
// (range checked against *(dt+0xB6C), stride 0x1A4); out of range or null unit calls 0xB1720(). t = D2Common#10737(unit); room = 0xA60D0(EAX = unit).
// R = row+0x58. t < word(row+0x96) - R, t > R and word row+0x1E > 0: 0xB8DF0(EAX = row+0x5C, EBX = unit; row+0x60, word 0x1E) != 0 calls 0xB28E0().
// R < t < R + 0x7D: walk the list at #10331(unit)+0x74 (next at +0xE8) over nodes with [0] == 2 and 0x11 <= [1] < 0x16:
//   word 0x1E > 0: 0xAFF10(ECX = node, EDX = room; word, 0x1630(node), 0x1660(node), #10240(unit), #11029(unit)) != 0 calls 0xB28E0().
//   word 0x20 > 0 and t % 8 == 0: id = #11018(unit); the pinned table at gl (128 slots, id & 0x7F, chain at +0xE4, key +0xC) finds node m;
//   id 0 or no m: #10637(unit, node[3]). m[0] != 2: the Fog assert (0xBF92(0x5A), 0xBF80, 0x3664(-1)). Else a zeroed 23-dword block
//   {0x421, room, 0, 0, word 0x20, 0x1630(node), 0x1660(node), 0x1630(m), 0x1660(m), 0, 0, #10240(unit), #11029(unit)} to 0xB30F0(ECX = block);
//   0x1330(EAX = 4, ECX = m+0x20) == 0 calls #10637(unit, node[3]).
// Then 0xB17C0(ECX = unit).
#include "cl_env.h"
extern "C" void d2c_bb8f0(uint32_t u, const cl::Env& e) {
    const uint32_t row = u ? e.Row((int32_t)e.R(u + 4)) : 0;
    if (!row) { e.H(0xb1720); return; }
    const int32_t t = (int32_t)e.H(0xc406, u);
    const uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    const int32_t r58 = (int32_t)e.R(row + 0x58);
    if (t < e.R16s(row + 0x96) - r58 && t > r58 && e.R16s(row + 0x1e) > 0) {
        if (e.H(0xb8df0, e.R(row + 0x60), (uint32_t)e.R16s(row + 0x1e), 0, 0, 0, e.R(row + 0x5c), 0, 0, u)) e.H(0xb28e0);
    }
    if (t < r58 + 0x7d && t > r58) {
        const uint32_t lst = e.H(0xc1f0, u);
        for (uint32_t n = e.R(lst + 0x74); n; n = e.R(n + 0xe8)) {
            if (e.R(n) != 2) continue;
            const uint32_t ty = e.R(n + 4);
            if (ty < 0x11 || ty >= 0x16) continue;
            if (e.R16s(row + 0x1e) > 0) {
                const uint32_t a3fa = e.H(0xc3fa, u);
                const uint32_t a388 = e.H(0xc388, u);
                const uint32_t a1660 = e.H(0x1660, 0, 0, 0, 0, 0, 0, n);
                const uint32_t a1630 = e.H(0x1630, 0, 0, 0, 0, 0, 0, n);
                if (e.H(0xaff10, (uint32_t)e.R16s(row + 0x1e), a1630, a1660, a388, a3fa, 0, n, room)) e.H(0xb28e0);
            }
            if (e.R16s(row + 0x20) <= 0) continue;
            if (t % 8 != 0) continue;
            const uint32_t id = e.H(0xc322, u);
            uint32_t m = 0;
            if (id) { for (m = e.R(e.gl + (id & 0x7f) * 4); m; m = e.R(m + 0xe4)) if (e.R(m + 0xc) == id) break; }
            if (!m) { e.H(0xc448, u, e.R(n + 0xc)); continue; }
            if (e.R(m) != 2) {
                const uint32_t x = e.H(0xbf92, 0x5a);
                e.H(0xbf80, 0xd359f, x);
                e.H(0x3664, 0xffffffffu);
                return;
            }
            uint32_t q[23] = {};
            q[0] = 0x421; q[1] = room;
            q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, n);
            q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, n);
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            q[4] = (uint32_t)e.R16s(row + 0x20);
            q[7] = e.H(0x1630, 0, 0, 0, 0, 0, 0, m);
            q[8] = e.H(0x1660, 0, 0, 0, 0, 0, 0, m);
            e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (!e.H(0x1330, 0, 0, 0, 0, 0, 4, m + 0x20)) e.H(0xc448, u, e.R(n + 0xc));
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
