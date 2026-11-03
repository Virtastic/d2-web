// 1.13c D2Client+0x74610 (350 B): flag the unit, send a type-0x21 message offset by the player's displacement, then notify.
// ECX = unit, EDX = skilldesc index, stack (ret 4): s0 = extra. tbl = *sgptDataTables (+0xB98 rows of 0x23C, +0xBA0 count,
// +0xB6C missile count). 0xA1340 takes EBX = unit and writes two outputs through EDI and s0; they sit at msg[7] and msg[8].
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
}

struct D2c74610Env {
    int32_t (*c40550)(uint32_t eax, uint32_t unit);        // 0x40550: EAX = index, stack = unit (ret 4): missile id
    uint32_t (*cC316)(uint32_t unit, uint32_t rec138, uint32_t idx, uint32_t extra);
    uint32_t (*a5f70)();                                   // 0xA5F70: player object
    uint32_t (*c1630)(uint32_t ecx);
    uint32_t (*c1660)(uint32_t ecx);
    uint32_t (*a1340)(uint32_t ebx, uint32_t* edi_out, uint32_t* s0_out);
    uint32_t (*b30f0)(const uint32_t* msg);
    void (*cC448)(uint32_t obj, uint32_t v);
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74610(uint32_t unit, int32_t idx, uint32_t extra, uint32_t tbl, const D2c74610Env* e) {
    if (idx < 0 || idx >= (int32_t)rd32(tbl + 0xba0)) return 0;
    uint32_t rec = (uint32_t)idx * 0x23c + rd32(tbl + 0xb98);
    if (!rec) return 0;
    int32_t id = e->c40550((uint32_t)idx, unit);
    if (id < 0 || id >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    if (unit) wr32(unit + 0xc4, rd32(unit + 0xc4) | 0x40);
    uint32_t r316 = e->cC316(unit, rd32(rec + 0x138), (uint32_t)idx, extra);
    uint32_t dx = 0, dy = 0;
    uint32_t pl = e->a5f70();
    if (pl) {
        dx = e->c1630(pl) - e->c1630(unit);
        dy = e->c1660(pl) - e->c1660(unit);
    }
    uint32_t m[23]; memset(m, 0, sizeof m);
    m[0] = 0x21; m[1] = unit;
    m[5] = e->c1630(unit) + dx;
    m[6] = e->c1660(unit) + dy;
    m[11] = (uint32_t)idx; m[4] = (uint32_t)id; m[12] = extra;
    if (e->a1340(unit, &m[7], &m[8]) && m[7] && m[8]) {
        uint32_t o = e->b30f0(m);
        if (o) { e->cC448(o, r316); return 1; }
    }
    return 0;
}
