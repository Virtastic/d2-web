// Seam for D2Game+0x6F8A0 (register convention: EAX = unit, EDI = game, stack = stamp, ret 4): insert a unit into the per-type server list.
#pragma once
#include <cstdint>
struct D2Game6F8A0Env {
    void (*stamp)(uint32_t unit);                        // 0xB10E (D2Common ordinal thunk), one stack word = unit, ret 4
    void (*detach)(uint32_t game, uint32_t unit);        // 0x8E880, EAX = game, stack = unit (ret 4)
    uint32_t (*node)(uint32_t game, uint32_t unit);      // 0x8E680, EAX = game, ECX = unit, returns the list node
    uint32_t (*typeSlot)(uint32_t type);                 // the dword table at module RVA 0xFA3DC, indexed by unit type
};
void d2game_port_6f8a0(uint32_t unit, uint32_t game, uint32_t stamp, const D2Game6F8A0Env* env);
