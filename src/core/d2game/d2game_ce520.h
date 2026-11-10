// Seams of src/core/d2game/d2game_ce520.cpp (shared with the FN probe in src/core/fn), in retail register/stack order.
#pragma once
#include <cstdint>

struct D2GameCE520Env {
    uint32_t (*f_cbf50)(uint32_t eax, uint32_t esi, uint32_t s0, uint32_t s1, uint32_t* s2, uint32_t* s3);   // ret 0x10; writes *s2, *s3
    uint32_t (*f_a3b2)(uint32_t unit);                                  // import thunk 0xA3B2 (1 word, ret 4)
    uint32_t (*f_a3e2)(uint32_t unit, uint32_t id, uint32_t z);         // import thunk 0xA3E2 (3 words)
    void (*f_a472)(uint32_t unit, uint32_t id, uint32_t v, uint32_t z); // import thunk 0xA472 (4 words)
    void (*f_cdf30)(uint32_t eax, uint32_t ecx, uint32_t s0, uint32_t s1, uint32_t s2);   // EAX = skill, ECX = unit, ret 0xC
};

// ECX = game, EDX = unit, stack = packet pointer, length; ret 8.
uint32_t d2game_ce520_port(uint32_t game, uint32_t unit, uint32_t pkt, uint32_t len, const D2GameCE520Env* e);
