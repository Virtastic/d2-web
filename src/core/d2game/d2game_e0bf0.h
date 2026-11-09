// 1.13c D2Game+0xE0BF0: release the unit a slot struct points at. EAX = slot struct S, stack: game (ret 4). 32-bit retail words.
// Callees are seams: findUnit = +0xE03A0 (ECX game, EAX type, EDX guid); detach = +0x48300 (ESI, EDI = S, stack game);
// unitCleanup = +0x8FAF0 (EAX unit, ECX S, stack game); a4a2 = stdcall(unit, 0); b168 = ECX unit, returns a pointer to a pointer.
#pragma once
#include <cstdint>
struct D2GameE0BF0Env {
    uint32_t (*findUnit)(uint32_t game, uint32_t type, uint32_t guid);
    void (*detach)(uint32_t esi, uint32_t edi, uint32_t game);
    uint32_t (*unitCleanup)(uint32_t unit, uint32_t slot, uint32_t game);
    uint32_t (*a4a2)(uint32_t unit, uint32_t zero);
    uint32_t (*b168)(uint32_t unit);
};
uint32_t d2game_e0bf0_port(uint32_t eax, uint32_t game, const D2GameE0BF0Env* e);
