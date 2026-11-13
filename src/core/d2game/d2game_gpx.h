// Shared helper of the 1.13c D2Game struct ports that call function-pointer slots inside capture blobs: the synthetic recorder rva of a slot
// address (0xF00000 + blob index * 0x10000 + offset, the rva the descriptor's `stubptr` lines log). 0 when the slot is in no blob.
#pragma once
#include "../fn/d2game_gp.h"
namespace d2g {
inline uint32_t slotRva(GE& e, uint32_t p)
{
    for (int k = 2; k < e.nb; k++) { const uint32_t b = e.B(k); if (p >= b && p < b + fnBlobLen(e.r, k)) return 0xF00000u + (uint32_t)k * 0x10000u + (p - b); }
    return 0;
}
}
