// 1.13c D2Client+0x61D00 (365 B, ret 8): stack (unit, arg1). Walks the chain [unit+0x74], next [node+0xE8]. A node runs when [11C200] != 0,
// or its flag bit 28 is set and bit 29 clear; it then gets bit 29 (0x20000000) in [node+0xC4]. Type [node] 1 (monster): id [node+4] through
// sgptDataTables (+0xA78 monster rows of 0x1A8 with a short at +0x18 = type index, count +0xA80; +0xA90 type rows of 0x134, count +0xA98;
// +0x118 is the value); type 2: Ordinal_10688(id)->[+0x1BC], with id 0x10B gated on Ordinal_10459(Ordinal_10826(unit)) in {2,3}, 0x16E on
// [node+0x10] == 2, 0x192 on Ordinal_10826(unit) == 0x4A. Each hit calls 61780(value, arg1).
#include <emscripten.h>
#include <cstdint>
typedef uint32_t u32;
static inline u32 r32(u32 a) { return *(const u32*)(uintptr_t)a; }
struct D2c61d00Env {
    u32 (*ord10688)(int32_t id);
    u32 (*ord10826)(u32 unit);
    u32 (*ord10459)(u32 v);
    u32 (*f61780)(u32 value, u32 arg1);
};
// Returns retail's EAX: the unit when the chain is empty, else the residue of the last node's path (bit 28 when the flag gate skips it,
// type - 2 for another type, the failed id / type index, the 10688 record when its value is 0, the gate callee's result, or 61780's).
extern "C" EMSCRIPTEN_KEEPALIVE
u32 d2c_61d00(u32 unit, u32 arg1, u32 c200, u32 dt, const D2c61d00Env* e) {
    u32 eax = unit;
    for (u32 n = r32(unit + 0x74); n; n = r32(n + 0xe8)) {
        u32* fl = (u32*)(uintptr_t)(n + 0xc4);
        eax = (*fl >> 0x1c) & 1;
        if (!c200) {
            if (!eax) continue;
            if ((*fl >> 0x1d) & 1) continue;
        }
        *fl |= 0x20000000;
        u32 type = r32(n);
        u32 v;
        eax = type - 1;
        if (type == 1) {
            int32_t id = (int32_t)r32(n + 4);
            eax = (u32)id;
            if (id < 0 || id >= (int32_t)r32(dt + 0xa80)) continue;
            int32_t ti = (int32_t)(int16_t)(*(const uint16_t*)(uintptr_t)(r32(dt + 0xa78) + (u32)id * 0x1a8 + 0x18));
            eax = (u32)ti;
            if (ti < 0 || ti >= (int32_t)r32(dt + 0xa98)) continue;
            u32 row = (u32)ti * 0x134 + r32(dt + 0xa90);
            eax = row;
            if (!row) continue;
            v = r32(row + 0x118);
            eax = v;
            if (!v) continue;
        } else if (type == 2) {
            int32_t id = (int32_t)r32(n + 4);
            u32 rec = e->ord10688(id);
            eax = rec;
            v = r32(rec + 0x1bc);
            if (!v) continue;
            if (id == 0x10b) {
                u32 c = e->ord10459(e->ord10826(unit));
                eax = c;
                if ((c & 0xff) != 2 && (c & 0xff) != 3) continue;
                v = r32(rec + 0x1bc);
            } else if (id == 0x16e) {
                eax = 0;
                if (r32(n + 0x10) != 2) continue;
            } else if (id == 0x192) {
                eax = e->ord10826(unit);
                if (eax != 0x4a) continue;
                v = r32(rec + 0x1bc);
            }
        } else { eax = type - 2; continue; }
        eax = e->f61780(v, arg1);
    }
    return eax;
}
