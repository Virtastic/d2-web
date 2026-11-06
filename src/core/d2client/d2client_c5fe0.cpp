// 1.13c D2Client+0xC5FE0: EAX = row index, stack = a, b (ret 8). dt = *sgptDataTables (rows dt+0xB98, count dt+0xBA0, stride 0x23C), bm = *gdwBitMasks.
// Index outside [0, count) or a null row gives 0. 0xC1F0(a) = h and 0x1400(EAX = h, EBX = b) = r must be nonzero. (bm[0] & row[5]) != 0 gives 1;
// otherwise 1 unless 0xC208(r) is nonzero.
#include <cstdint>
extern "C" uint32_t d2c_port_c5fe0(int32_t idx, uint32_t a, uint32_t b, const uint8_t* dt, const uint8_t* bm,
                                   uint32_t (*c1f0)(uint32_t), uint32_t (*f1400)(uint32_t, uint32_t), uint32_t (*c208)(uint32_t)) {
    if (idx < 0 || idx >= *(const int32_t*)(dt + 0xBA0)) return 0;
    uint32_t row = (uint32_t)idx * 0x23Cu + *(const uint32_t*)(dt + 0xB98);
    if (!row) return 0;
    uint32_t h = c1f0(a);
    if (!h) return 0;
    uint32_t r = f1400(h, b);
    if (!r) return 0;
    if (bm[0] & *(const uint8_t*)(uintptr_t)(row + 5)) return 1;
    return c208(r) ? 0u : 1u;
}
