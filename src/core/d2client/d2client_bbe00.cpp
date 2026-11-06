// 1.13c D2Client+0xBBE00: ECX = object (dword +4 = row index), dt = *sgptDataTables (rows dt+0xB64, count dt+0xB6C, stride 0x1A4).
// A valid row runs 0xB9470 with the sign-extended +0x1E; 0xB17C0 always runs last.
#include <cstdint>
namespace {
static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline uint32_t d2c_bb_row(uint32_t self, const uint8_t* dt) {
    if (!self) return 0;
    int32_t idx = (int32_t)rd32(self + 4);
    if (idx < 0 || idx >= *(const int32_t*)(dt + 0xB6C)) return 0;
    return (uint32_t)idx * 0x1A4u + *(const uint32_t*)(dt + 0xB64);
}
}
extern "C" void d2c_port_bbe00(uint32_t self, const uint8_t* dt, void (*b9470)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t), void (*b17c0)(uint32_t)) {
    uint32_t row = d2c_bb_row(self, dt);
    if (row) b9470(self, rd32(row + 0x58), rd32(row + 0x5C), rd32(row + 0x60), (int32_t)(int16_t)*(const uint16_t*)(uintptr_t)(row + 0x1E));
    b17c0(self);
}
