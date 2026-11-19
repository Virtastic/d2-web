// 1.13c D2Game+0x22A10 (28 B, EAX = unit, one stack word): set the monster's boss index word (monster data +0x26).
// No-op for a null unit, a unit that is not a monster (type 1) and a unit with no data pointer (+0x14). Retail layout (32-bit words).
#include <cstdint>
#include <cstring>

// 1.13c D2Game+0x22A10
void D2Game_22A10(void* pUnit, uint16_t nBossHcIdx)
{
    if (pUnit == nullptr) return;
    uint32_t type, data;
    std::memcpy(&type, pUnit, 4);
    if (type != 1) return;
    std::memcpy(&data, static_cast<uint8_t*>(pUnit) + 0x14, 4);
    if (data == 0) return;
    std::memcpy(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(data)) + 0x26, &nBossHcIdx, 2);
}
