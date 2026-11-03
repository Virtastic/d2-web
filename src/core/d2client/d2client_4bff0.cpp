// 1.13c D2Client+0x4BFF0: EDI = unit. The player mode-row selector: a unit of type 1 with mode idx < 0x10 picks a
// 12-byte row of one of four static tables. Callee seams: 0x1260 (EAX = unit class, returns the class record or null) and D2Common ord 10707 (thunk 0xCDA2, stdcall 2 words).
// The four tables are DAT_6FBA21A8 / 6FBA2268 (16 rows) and the single rows 6FBA22B0 / 6FBA22BC.
#include <emscripten.h>
#include <cstdint>
struct D2c4bff0Env {
    const uint8_t* (*f1260)(int32_t cls);                       // record with the per-mode bit set at +0x104
    int32_t (*o10707)(const int32_t* unit, int32_t idx);
    const uint8_t* bitMasks;                                    // gdwBitMasks (D2Common): dword per bit, only the low byte is tested
};
namespace {
const int32_t kT21a8[16][3] = { {0,2,0},{0,0,0},{1,0,1},{0,1,2},{2,1,3},{2,1,3},{0,1,2},{0,1,2},{0,1,2},{0,1,2},{0,2,0},{0,0,0},{0,2,0},{1,2,2},{2,1,2},{1,0,1} };
const int32_t kT2268[16][3] = { {1,2,0},{0,1,2},{0,1,2},{0,1,2},{1,0,4},{1,0,4},{2,1,3},{1,0,4},{1,0,4},{1,0,4},{0,2,0},{1,2,0},{0,1,2},{0,1,2},{0,1,2},{0,1,2} };
const int32_t kT22b0[3] = { 2,1,3 };
const int32_t kT22bc[3] = { 1,0,4 };
}
extern "C" const void* d2_client_4bff0(const int32_t* u, const D2c4bff0Env* e) {
    if (!u || u[0] != 1) return nullptr;
    int32_t idx = u[4];
    if (idx >= 0x10) return nullptr;
    int32_t cls = u[1];
    if ((cls == 0xf3 || cls == 0x14d) && idx == 0xb) return kT22b0;
    if ((cls == 0x21f && idx == 10) || (cls == 0x220 && idx == 10) || (cls == 0x23a && idx == 10)) return kT22bc;
    const uint8_t* rec = e->f1260(cls);
    if (!rec) return nullptr;
    if (e->bitMasks[(idx & 7) * 4] & rec[(idx >> 3) + 0x104]) return kT2268[idx];
    if (idx == 0 && e->o10707(u, idx)) return kT2268[0];
    return kT21a8[idx];
}
