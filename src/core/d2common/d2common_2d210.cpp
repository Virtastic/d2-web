// 1.13c D2Common+0x2D210: set one slot of a DRLG warp record (thiscall: EAX = warp record, EBX = id, ECX = slot or -1 for
// the first free slot, stack word = value). Words 1..8 hold the ids, words 9..16 the values. An id already present takes
// the value in place; otherwise the slot (or the first free one, id 0 and value -1) takes both. With no free slot the write
// falls back to word 0 (id) and word 8 (value).
#include <cstdint>

extern "C" void D2Common_2D210_SetWarpSlot(uint32_t warp, uint32_t id, uint32_t slot, uint32_t value)
{
    int32_t* w = reinterpret_cast<int32_t*>(static_cast<uintptr_t>(warp));
    for (int32_t i = 0; i < 8; ++i)
        if (w[1 + i] == static_cast<int32_t>(id)) { w[9 + i] = static_cast<int32_t>(value); return; }
    int32_t k = static_cast<int32_t>(slot);
    if (k == -1) {
        for (int32_t i = 0; i < 8; ++i)
            if (w[1 + i] == 0 && w[9 + i] == -1) { k = i; break; }
    }
    w[k + 1] = static_cast<int32_t>(id);
    w[k + 9] = static_cast<int32_t>(value);
}
