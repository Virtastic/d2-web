// 1.13c D2Client+0x74930 (396 B): type-0x8020 message for skilldesc index s0 on the unit in EAX, with a repeat count, then the
// unit's state is rewritten. EAX = unit, stack (ret 8): s0 = skilldesc index, s1 = extra. tbl = *sgptDataTables (+0xB98/+0xBA0
// skilldesc rows of 0x23C, +0xB6C missile count). 0xA1340 (EBX = unit) writes the y output through s0 and the x output through
// EDI; x sits below y in the frame and ends up in msg[7], y in msg[8].
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline void wr32(uint32_t a, uint32_t v) { *(uint32_t*)(uintptr_t)a = v; }
inline int32_t rs16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
}

struct D2c74930Env {
    uint32_t (*a1340)(uint32_t ebx, uint32_t* edi_out, uint32_t* s0_out);
    uint32_t (*c1330)(uint32_t eax, uint32_t ecx);         // EAX = 2, ECX = unit + 0x20
    uint32_t (*cC316)(uint32_t unit, uint32_t rec138, uint32_t idx, uint32_t extra);
    uint32_t (*b30f0)(const uint32_t* msg);
    void (*f73bc0)(uint32_t esi, uint32_t edi);            // ESI = the new object, EDI = the count
    uint32_t (*cC1FC)(uint32_t unit, uint32_t v);
    void (*cCB56)(uint32_t unit, uint32_t v);
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74930(uint32_t unit, int32_t idx, uint32_t extra, uint32_t tbl, const D2c74930Env* e) {
    if (idx < 0 || idx >= (int32_t)rd32(tbl + 0xba0)) return 0;
    uint32_t rec = (uint32_t)idx * 0x23c + rd32(tbl + 0xb98);
    if (!rec) return 0;
    int32_t id = rs16(rec + 0xea);
    if (id < 0 || id >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    int32_t alt = rs16(rec + 0xec);
    if (alt < 0 || alt >= (int32_t)rd32(tbl + 0xb6c)) alt = -1;
    uint32_t xy[2] = { 0, 0 };                             // [0] = EDI's output (x), [1] = s0's output (y)
    if (!e->a1340(unit, &xy[0], &xy[1])) return 0;
    uint32_t mid = (uint32_t)id;
    if (alt >= 0 && e->c1330(2, unit + 0x20)) mid = (uint32_t)alt;
    int32_t cnt = (int32_t)e->cC316(unit, rd32(rec + 0x138), (uint32_t)idx, extra);
    if (cnt < 1) cnt = 1;
    uint32_t m[23]; memset(m, 0, sizeof m);
    m[4] = mid; m[7] = xy[0]; m[8] = xy[1];
    m[0] = 0x8020; m[1] = unit; m[2] = unit;
    m[11] = (uint32_t)idx; m[12] = extra; m[19] = (uint32_t)cnt;
    uint32_t o = e->b30f0(m);
    if (o) e->f73bc0(o, (uint32_t)cnt);
    if (e->cC1FC(unit, 0xc)) {
        if (rd32(unit + 0x30)) { e->cCB56(unit, 8); wr32(unit + 0x48, 0x500); return o; }
        wr32(unit + 0x44, (rd32(unit + 0x44) & 0xffffff00u) - 0x200u);
    }
    return o;
}
