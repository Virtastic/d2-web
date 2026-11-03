// 1.13c D2Client+0x74480 (393 B): type-0x21 message for the skill read from a unit's state values. EBX = unit, ESI = live-in
// handed to the 0xC1C0 import as its third word; plain ret. tbl = *sgptDataTables. Callees (rva + 0x6FAB0000): imports C352,
// C1C0, C4B4, C1F0, C208 (stdcall thunks), 15B0, 1330, 1630, 1660, B30F0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline int32_t rs16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
}

struct D2c74480Env {
    uint32_t (*cC352)(uint32_t s0);
    uint32_t (*cC1C0)(uint32_t s0, uint32_t s1, uint32_t s2);
    int32_t (*cC4B4)(uint32_t s0, uint32_t s1, uint32_t s2);
    uint32_t (*c15b0)(uint32_t eax);
    uint32_t (*cC1F0)(uint32_t s0);
    uint32_t (*cC208)(uint32_t s0);
    uint32_t (*c1630)(uint32_t ecx);
    uint32_t (*c1660)(uint32_t ecx);
    uint32_t (*c1330)(uint32_t eax, uint32_t ecx);        // EAX = 2 or 3, ECX = unit + 0x20
    void (*b30f0)(const uint32_t* msg);
};

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_74480(uint32_t ebx, uint32_t esi, uint32_t tbl, const D2c74480Env* e) {
    uint32_t v = rd32(ebx + 0x2c);
    if (!v) return;
    if (!e->cC352(v)) return;
    uint32_t h = e->cC1C0(ebx, 0xd, esi);
    if (!h) return;
    int32_t n1 = e->cC4B4(h, 0x15e, 0);
    int32_t n2 = e->cC4B4(h, 0x15f, 0);
    if (n1 <= 0 || n1 >= (int32_t)rd32(tbl + 0xba0)) return;
    if (n2 < 0) n2 = 1;
    uint32_t rec = e->c15b0((uint32_t)n1);
    if (!rec) return;
    int32_t id = rs16(rec + 0xea);
    if (id < 0 || id >= (int32_t)rd32(tbl + 0xb6c)) return;
    if (e->cC208(e->cC1F0(ebx))) return;
    uint32_t m[23]; memset(m, 0, sizeof m);
    m[5] = e->c1630(ebx);
    m[0] = 0x21; m[1] = ebx;
    m[6] = e->c1660(ebx);
    m[4] = (uint32_t)id; m[11] = (uint32_t)n1; m[12] = (uint32_t)n2;
    if (rs16(rec + 0xec) >= 0) {
        if (rs16(rec + 0xee) < 0) {
            if (e->c1330(2, ebx + 0x20) == 0) m[4] = (uint32_t)rs16(rec + 0xec);
        } else {
            uint32_t r = e->c1330(3, ebx + 0x20);
            if (r == 0) m[4] = (uint32_t)rs16(rec + 0xec);
            else if (r == 1) m[4] = (uint32_t)rs16(rec + 0xee);
        }
    }
    e->b30f0(m);
}
