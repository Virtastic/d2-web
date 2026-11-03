// 1.13c D2Client+0x74210 (309 B): spawn-type-0x20 message for the player's selected skill. ECX = gate (must be 0),
// EDX = target unit, stack (ret 0x10): s0 = acting unit, s1 unused, s2 = skill id handed to 0x15B0, s3 = extra.
// Globals are parameters: tbl = *sgptDataTables (read at +0xB6C missile count, +0xBC0 skill count), masks = the bitmask table.
// Callees are the Env callbacks (retail addresses = rva + 0x6FAB0000).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline int32_t rs16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
}

struct D2c74210Env {
    uint32_t (*a60d0)(uint32_t eax);                       // 0xA60D0: unit to player object
    uint32_t (*a62a0)(uint32_t eax, uint32_t s0);          // 0xA62A0 (stdcall 1)
    uint32_t (*c1580)(uint32_t eax);                       // 0x1580: record with a flag byte at +5
    uint32_t (*c15b0)(uint32_t eax);                       // 0x15B0: skill record
    uint32_t (*c1630)(uint32_t ecx);
    uint32_t (*c1660)(uint32_t ecx);
    void (*b30f0)(const uint32_t* msg);                    // 0xB30F0 (ecx = 23-dword message)
    void (*b6b300)(uint32_t eax, uint32_t unit, uint32_t two);   // 0x6B300: EAX = skill, 7 stack words (unit, 2, 0 x5)
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74210(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s2, uint32_t s3,
                         uint32_t tbl, const uint8_t* masks, const D2c74210Env* e) {
    if (ecx != 0) return 0;
    if (edx == 0 || s0 == 0) return 0;
    uint32_t esi = e->a60d0(s0);
    if (!esi) return 0;
    if (!e->a62a0(esi, edx)) return 0;
    uint32_t r1580 = e->c1580(rd32(s0 + 4));
    if (!(masks[4] & *(const uint8_t*)(uintptr_t)(r1580 + 5))) return 0;
    uint32_t rec = e->c15b0(s2);
    if (!rec) return 0;
    int32_t id = rs16(rec + 0xea);
    if (id < 0 || id >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    uint32_t m[23]; memset(m, 0, sizeof m);
    m[0] = 0x20; m[1] = edx; m[2] = edx; m[4] = (uint32_t)id; m[11] = s2; m[12] = s3;
    m[7] = e->c1630(esi);
    m[8] = e->c1660(esi);
    e->b30f0(m);
    int32_t sk = rs16(rec + 0x110);
    if (sk >= 0 && sk < (int32_t)rd32(tbl + 0xbc0)) e->b6b300((uint32_t)sk, edx, 2);
    return 1;
}
