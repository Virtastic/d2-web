// 1.13c D2Client+0x74770 (435 B): two type-0x21 messages mirrored about the unit, then a type-1 message. ECX = unit, EDX = skilldesc
// index, stack (ret 4): s0 = extra. tbl = *sgptDataTables (+0xB98/+0xBA0 skilldesc, +0xB64/+0xB6C missile rows of 0x1A4),
// masks = *gdwBitMasks. 0xA1340 (EBX = unit) writes one output through EDI and one through s0; the two sit side by side in
// the frame, s0's below EDI's, so they are one pair here.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
inline int32_t rs16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
}

struct D2c74770Env {
    uint32_t (*a1340)(uint32_t ebx, uint32_t* edi_out, uint32_t* s0_out);
    uint32_t (*a1410)(uint32_t eax, uint32_t ecx);         // EAX = missile id, ECX = unit
    uint32_t (*cC1F0)(uint32_t unit);
    uint32_t (*c1400)(uint32_t eax, uint32_t ebx, uint32_t edi);   // EAX = the C1F0 result, EBX = EDI-out, EDI = s0-out
    uint32_t (*cC208)(uint32_t v);
    uint32_t (*c1630)(uint32_t ecx);
    uint32_t (*c1660)(uint32_t ecx);
    void (*b30f0)(const uint32_t* msg);
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_74770(uint32_t unit, int32_t idx, uint32_t extra, uint32_t tbl, const uint8_t* masks, const D2c74770Env* e) {
    if (idx < 0 || idx >= (int32_t)rd32(tbl + 0xba0)) return 0;
    uint32_t rec = (uint32_t)idx * 0x23c + rd32(tbl + 0xb98);
    if (!rec) return 0;
    int32_t id = rs16(rec + 0xea);
    if (id < 0 || id >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    uint32_t mrec = (uint32_t)id * 0x1a4 + rd32(tbl + 0xb64);
    if (!mrec) return 0;
    int32_t sk = rs16(mrec + 0x18);
    if (sk < 0 || sk >= (int32_t)rd32(tbl + 0xb6c)) return 0;
    uint32_t yx[2] = { 0, 0 };                             // [0] = s0's output (y), [1] = EDI's output (x)
    if (!e->a1340(unit, &yx[1], &yx[0])) return 0;
    if (!e->a1410((uint32_t)id, unit)) return 0;
    uint32_t h = e->cC1F0(unit);
    if (!h) return 0;
    uint32_t x = yx[1], y = yx[0];
    uint32_t r1400 = e->c1400(h, x, y);
    if ((masks[0] & *(const uint8_t*)(uintptr_t)(rec + 5)) == 0) {
        if (e->cC208(r1400)) return 0;
    }
    uint32_t m[23]; memset(m, 0, sizeof m);
    m[4] = (uint32_t)id; m[0] = 0x21; m[1] = unit; m[5] = x; m[6] = y;
    m[11] = (uint32_t)idx; m[12] = extra;
    uint32_t dx = e->c1630(unit) - x;
    uint32_t dy = e->c1660(unit) - y;
    m[7] = x - dy; m[8] = dx + y;   // retail [esp+0x48]/[esp+0x4c] of the block at esp+0x2c
    e->b30f0(m);
    m[7] = dy + x; m[8] = y - dx;
    e->b30f0(m);
    m[4] = (uint32_t)sk; m[7] = 0; m[8] = 0; m[0] = 1;
    e->b30f0(m);
    return 1;
}
