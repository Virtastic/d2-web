// 1.13c D2Client+0x2D6E0 (262 B, AL = kind, ESI = unit, EBX / EDI = accumulators, plain ret): kind 1 adds stats 0x30 / 0x31,
// 2 adds 0x32 / 0x33, 3 adds 0x34 / 0x35, 4 and 12 add 0x36 / 0x37 to *ebx / *edi (GetUnitStat thunk 0xC1C6, (unit, stat, 0)).
// Kind 5: m = stat 0x65, or when that is <= 0, stat 0x3B / max(stat 0x146, 1); adds (stat 0x39 * m) >> 8 and (stat 0x3A * m) >> 8.
// Any other kind does nothing.
#include <cstdint>
typedef int32_t (*D2Client2D6E0Stat)(const void* unit, uint32_t stat, uint32_t z);
extern "C" void d2c_port_2d6e0(uint32_t kind, const void* unit, int32_t* ebx, int32_t* edi, D2Client2D6E0Stat stat) {
    static const uint8_t idx[12] = { 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 3 };
    static const uint32_t pair[4] = { 0x30, 0x32, 0x34, 0x36 };
    uint32_t k = (uint8_t)kind - 1u;
    if (k > 11) return;
    uint32_t c = idx[k];
    if (c < 4) {
        int32_t a = stat(unit, pair[c], 0); *ebx = (int32_t)((uint32_t)*ebx + (uint32_t)a);
        int32_t b = stat(unit, pair[c] + 1, 0); *edi = (int32_t)((uint32_t)*edi + (uint32_t)b);
        return;
    }
    if (c != 4) return;
    int32_t m = stat(unit, 0x65, 0);
    if (m <= 0) {
        int32_t d = stat(unit, 0x146, 0); if (d <= 1) d = 1;
        int32_t n = stat(unit, 0x3B, 0);
        m = (d == 1) ? n : n / d;
    }
    int32_t p = stat(unit, 0x39, 0), q = stat(unit, 0x3A, 0);
    *ebx = (int32_t)((uint32_t)*ebx + (uint32_t)((int32_t)((uint32_t)p * (uint32_t)m) >> 8));
    *edi = (int32_t)((uint32_t)*edi + (uint32_t)((int32_t)((uint32_t)q * (uint32_t)m) >> 8));
}
