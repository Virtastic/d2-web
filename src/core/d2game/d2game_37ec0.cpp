// D2Game item-to-player hand-off check in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2Game37EC0Env {
    uint32_t (*itemOwnerValue)(uint32_t item);                                   // 0x1100: ECX = unit
    // 0xD4E80: ECX = player, EDX = a1, EAX = unit id; stack (value, a2, &a2), ret 0xC
    uint32_t (*place)(uint32_t player, uint32_t a1, uint32_t unitId, uint32_t value, uint32_t a2, uint32_t* pA2);
};

// 1.13c D2Game+0x37EC0
// ECX = item, EDI = player; stack a1, a2 (ret 8). A null item exits through Fog with 0xD8, a null player
// with 0xD9. The item id (+0xC) is read before 0x1100(item); 0xD4E80 then gets the player, a1, the id, the
// 0x1100 result, a2 and the address of the caller's a2 slot. Result: 1 when 0xD4E80 returned non-zero.
uint32_t ITEM_PlaceForPlayer_37EC0(uint32_t item, uint32_t player, uint32_t a1, uint32_t a2, const D2Game37EC0Env& env)
{
    if (item == 0 || player == 0) _exit(-1);   // Fog fatal 0xD8 / 0xD9
    uint32_t unitId; std::memcpy(&unitId, (const void*)(uintptr_t)(item + 0xC), 4);
    const uint32_t value = env.itemOwnerValue(item);
    return env.place(player, a1, unitId, value, a2, &a2) != 0 ? 1u : 0u;
}
