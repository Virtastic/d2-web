// src/core/d2client/unit_create_a7780.cpp -- D2Client.dll 1.13c, base 0x6FAB0000. Mapping lane, env-seam style.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

// host rva: < 0x10000 = D2Client RVA, 0x10000|n = import ordinal n. Register/stack conventions of the
// ordinal calls are UNVERIFIED (a1..): args go in s[] in decompiler order.
// Globals: DAT_6fbcbbfc=+0x10BBFC (player unit), DAT_6fbcc4d4=+0x1104D4 (client-unit list head, next at +0x30,
// type +4, id +8, key +0xC), DAT_6fbba808=+0x10A808 (unit hash[128], chain next at word 0x39), DAT_6fbcc390=+0x110390
// (per-class table), sgptDataTables (item row 0x1A8 @+0xA78/count +0xA80; 0x134 rows @+0xA90/count +0xA98).
struct A7780G {
    const uint8_t* player;          // *DAT_6fbcbbfc, 0 if none
    uint32_t listHead;              // DAT_6fbcc4d4
    uint32_t* hash[128];            // DAT_6fbba808
    const uint8_t* dataTables;      // *sgptDataTables
    const uint8_t* classTab;        // DAT_6fbcc390
};
static uint32_t H(D2cHost h, uint32_t rva, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) {
    D2cCall k = {}; k.rva = rva; k.s[0] = a; k.s[1] = b; k.s[2] = c; k.s[3] = d; return h(&k);
}
// 1.13c D2Client+0xA7780 (1123 B; callers 0xAE420/0x1E070/0xA7BF0/0x181A0): create (or re-use) a client item unit
// from a received item-spec p (p[0]=id p[1]=txtIdx p[2]=seed p[4]/p[5]=flags/colour, bytes 0..8 copied to data+0x1C).
// chainWalk(): ordinal 10265/10024/_exit fatal-error paths are folded into return -1 (UNVERIFIED mapping).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_a7780(uint32_t* p, uint16_t a2, uint16_t a3, int32_t a4,
                                                        const uint32_t* rect, A7780G* g, D2cHost h) {
    uint32_t want = 0xFFFFFFFFu;
    if (g->player)
        for (uint32_t n = g->listHead; n; ) {   // list nodes are host memory; seam reads via host(rva 0)
            D2cCall k = {}; k.rva = 0; k.eax = n; h(&k);   // out[0]=type out[1]=id out[2]=key out[3]=next
            if (k.out[0] == 7 && k.out[2] == *(const uint32_t*)(g->player + 0xC)) { want = k.out[1]; break; }
            n = k.out[3];
        }
    uint32_t id = p[0];
    if (id == want)
        for (uint32_t* u = g->hash[id & 0x7F]; u; u = (uint32_t*)(uintptr_t)u[0x39])
            if (u[3] == id) {
                if (u[0] != 1) return -1;                       // fatal (ordinal 10265(0x5a)+10024+_exit)
                H(h, 0x69700); H(h, 0x66950);                   // FUN_6fb19700, FUN_6fb16950
                H(h, 0x10000 | 11090, (uint32_t)(uintptr_t)u, p[6]);
                return (int32_t)(uintptr_t)u;
            }
    int32_t tx = (int32_t)p[1];
    if (tx < 0 || tx >= *(const int32_t*)(g->dataTables + 0xA80)) return 0;
    const uint8_t* row = *(const uint8_t* const*)(g->dataTables + 0xA78) + (uint32_t)tx * 0x1A8;
    int32_t t2 = *(const int16_t*)(row + 0x18);
    if (t2 < 0 || t2 >= *(const int32_t*)(g->dataTables + 0xA98)) return 0;
    uint32_t* u = (uint32_t*)(uintptr_t)H(h, 0x10000 | 11153, 0, 1);   // alloc unit (type 0, kind 1)
    if (a4 && u) u[0x31] |= 0x600000;
    u[0] = 1; u[1] = p[1]; u[3] = p[0]; u[2] = 0; u[0x35] = 0;
    uint32_t rs = H(h, 0xA5A60, p[2]);                              // FUN_6fb55a60 seed holder
    H(h, 0x10000 | 10936);
    if (rs) { uint64_t v = (uint64_t)*(uint32_t*)(uintptr_t)(rs + 0x6C) * 0x6AC690C5ull + *(uint32_t*)(uintptr_t)(rs + 0x70);
              *(uint64_t*)(uintptr_t)(rs + 0x6C) = v; u[10] = (uint32_t)v; }
    H(h, 0x10000 | 10614); H(h, 0x4C370, 0);                        // FUN_6fafc370(ecx,0)
    uint8_t* d = u[5] ? (uint8_t*)(uintptr_t)u[5] : nullptr;        // item data (u[0]==1)
    if (d) *(uint16_t*)(d + 0x16) |= (uint16_t)p[4];
    if (d && (d[0x16] & 2)) *(uint16_t*)(d + 0x26) = a3;
    if (d) *(uint32_t*)(d + 0x3C) = p[5];
    H(h, 0x10000 | 10003); H(h, 0x10000 | 10971);
    if (d) {
        *(uint16_t*)(d + 0x14) = a2;
        const uint8_t* s = (const uint8_t*)p; for (int i = 0; i < 9; i++) d[0x1C + i] = s[i];
    }
    if (rect && d) { uint32_t* r = (uint32_t*)(d + 4); r[0] = rect[0]; r[1] = rect[1]; r[2] = rect[2]; r[3] = rect[3]; }
    H(h, 0x4D160);                                                  // FUN_6fafd160
    H(h, 0x10000 | 11005, u[0xB], row[0xE]); H(h, 0x66950); H(h, 0x570C0, (uint32_t)(uintptr_t)u);
    uint32_t col = 2; uint8_t b4d = ((uint8_t*)u)[0x4D]; if (b4d < 8) col = b4d + 2;
    if (d && (d[0x16] & 8) && H(h, 0x1510, 0xF) == 0) col = H(h, 0x539E0);
    uint32_t ix = H(h, 0x1260);
    if (ix) { uint8_t b = g->classTab[0x122 + ix]; if (b) { col = b; if (col == 0xFF) col = H(h, 0x21B10) == 0; } }
    if (d && (d[0x16] & 2)) { int16_t s = *(int16_t*)(d + 0x26); uint32_t q = H(h, 0x10000 | 10038, (uint32_t)(int32_t)s);
        uint8_t b = g->classTab[0x28 + q]; if (b) col = b; }
    if ((int32_t)col > 0x1D) col = 2;
    if (!u[0x15]) return -1;                                        // fatal 0xCE4 / 0xCFD
    ((uint32_t*)(uintptr_t)u[0x15])[0xD] = col; ((uint32_t*)(uintptr_t)u[0x15])[0xE] = col;
    H(h, 0x57360, col);                                             // FUN_6fb57360 -> piVar12 (key at +0xC); tail:
    H(h, 0x10000 | 11104, 0); H(h, 0x23170);                        // list-match + FUN_6fafef50 branch folded to host
    return 0;                                                       // retail returns piVar15 (= 0 on this path)
}
