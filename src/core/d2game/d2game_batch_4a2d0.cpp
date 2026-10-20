// 1.13c D2Game+0x4A2D0 -- sub_4A2D0 (batch lane: implementation + marker only).
// Retail register args: edx=step (param_2), esi=record, edi=base. Bits 8..12 of the u16 at +10 are a
// 5-bit counter; bit 5 selects stride 5 (set) or 4 (clear). Counter is replaced by base+stride*step
// only if it is <= that value.
#include <cstdint>
#include <cstring>

void D2Game_sub_4A2D0(void* rec, int32_t base, int32_t step)
{
    uint16_t w;
    std::memcpy(&w, (uint8_t*)rec + 10, 2);
    const uint32_t stride = ((w & 0x20u) | 0x80u) >> 5;
    if ((int32_t)((w & 0x1f00u) >> 8) <= (int32_t)(stride * step + base)) {
        w = (uint16_t)((uint16_t)(step * stride + base) * 0x100 | (w & 0xe0ff));
        std::memcpy((uint8_t*)rec + 10, &w, 2);
    }
}
