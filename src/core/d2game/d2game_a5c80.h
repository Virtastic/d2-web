// D2Game+0xA5C80 seams: the AI "cast skill 8 unless queued" step; each callee is reachable through the table so the FN replay can hook it.
#pragma once
#include <cstdint>
struct D2GameA5C80Env {
    void (*lookupRef)(void* unit, uint32_t* ref);                       // 0x21490 (EAX = unit, stack &ref, 0); writes *ref
    void (*allocCommand)(void* game, void* unit, const uint32_t* blk);  // 0x212F0 (EAX = unit, stack game, &block)
    void (*freeMinions)(void* unit);                                    // 0x216A0 (EAX = unit)
    void (*executeAi)(void* game, void* unit, uint32_t a, uint32_t b);  // 0x9D230 (ECX = game, EDX = unit, stack a, b)
    void (*defaultAction)(void* game, int action);                      // 0xF22C0 (EAX = game)
};
void d2game_a5c80(const D2GameA5C80Env& env, void* game, uint8_t* unit, const uint8_t* ctx);
