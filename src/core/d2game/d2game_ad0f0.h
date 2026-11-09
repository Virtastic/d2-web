// D2Game+0xAD0F0 seams: use an item through its item-type handler table, each callee reachable through the table so the FN replay can hook it.
#pragma once
#include <cstdint>
struct D2GameAD0F0Env {
    uint32_t (*itemIsType)(void* item, uint32_t type);                 // 0xA430 D2Common 10744 (stack item, type), ret 8
    uint32_t (*itemField)(void* item, uint32_t n);                     // 0xA90A D2Common 11028 (stack item, n), ret 8; only AX is used
    void* (*useRecord)(uint32_t idx);                                  // 0xA9D6 D2Common 11065 (stack idx), ret 4
    void* (*itemRecord)(uint32_t classId);                             // 0xA53E D2Common 10695 (stack class id), ret 4
    uint32_t (*itemFlagTest)(void* item, uint32_t flag, uint32_t line);// 0xA8A4 D2Common 10202 (stack item, flag, line, file), ret 0x10
    void (*itemFlagSet)(void* item, uint32_t flag, uint32_t on);       // 0xA65E D2Common 10977 (stack item, flag, on), ret 0xC
    void* (*playerData)(void* unit);                                   // 0xA4C0 D2Common 10920 (stack unit), ret 4
    void (*afterUse)(void* unit);                                      // 0xACF10 (stack unit), ret 4
    void (*send)(uint32_t eax, uint32_t ecx, uint32_t id, uint32_t x);   // 0x8C3B0 (AL op, CL type, EDX id, stack x), ret 4
    // a handler of the table: ECX game, EDX unit, stack item, a, b, c, d; ret 0x14. rva names which one.
    uint32_t (*handler)(uint32_t rva, uint32_t game, void* unit, void* item, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
};
// EDI = unit, ESI = item; stack game, a1, a2, a3 (ret 0x10).
uint32_t d2game_ad0f0(const D2GameAD0F0Env& env, void* unit, void* item, uint32_t game, uint32_t a1, uint32_t a2, uint32_t a3);
