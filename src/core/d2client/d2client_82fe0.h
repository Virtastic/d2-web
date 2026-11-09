// 1.13c D2Client+0x82FE0: reconcile a unit's equipped items after an inventory change (four passes over the inventory).
// Retail words (unit, item, node, inventory) are 32-bit addresses; members are named by the thunk RVA they stand for.
#pragma once
#include <cstdint>
struct D2c82fe0Env {
    void     (*f81b90)(uint32_t eax, uint8_t* esi);                 // EAX = unit, ESI = the 0x1C-byte local block
    uint32_t (*c4f0)(uint32_t inv);                                 // first inventory node
    uint32_t (*c526)(uint32_t node);                                // item of a node
    uint32_t (*c994)(uint32_t node);                                // node kind (1 = equipped)
    uint32_t (*ca30)(uint32_t item, uint32_t unit);
    uint32_t (*c51a)(uint32_t item, uint32_t zero);
    uint32_t (*f81c30)(uint32_t eax, uint32_t ebx);                 // EAX = unit, EBX = item
    void     (*ca42)(uint32_t item, uint32_t flag, uint32_t on);
    void     (*f81ff0)(uint32_t edi, uint32_t esi, uint32_t s0);    // EDI = unit, ESI = item
    uint32_t (*c568)(uint32_t node);                                // next node
    uint32_t (*c262)(uint32_t inv, uint32_t slot);                  // item in body slot
    uint32_t (*c1a8)(uint32_t item, uint32_t flag, uint32_t line, uint32_t file);
    uint32_t (*c9f4)(uint32_t unit, uint32_t item);                 // low byte used
    void     (*f82610)(uint32_t esi, uint32_t s0);                  // ESI = unit
    uint32_t (*c898)(uint32_t item, uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    uint32_t (*c928)(uint32_t item);
    uint32_t (*c1cc)(uint32_t item, uint32_t need);
    uint32_t (*ca54)(uint32_t item);                                // low byte used
    void     (*ca2a)(uint32_t inv, uint32_t item);
    void     (*f69a90)(uint32_t eax, uint32_t ecx);                 // EAX = byte from C9F4, ECX = unit
    void     (*ca18)(uint32_t inv, uint32_t item);
    uint32_t (*c19c)(uint32_t item);
    void     (*ca0c)(uint32_t inv);
    void     (*c544)(uint32_t unit, uint32_t item);
    void     (*f82410)(uint32_t eax, uint8_t* edi);                 // EAX = unit, EDI = the local block
    void     (*f19830)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1);
    void     (*f6a220)(uint32_t eax, uint32_t s0);
    void     (*f4dc70)(uint32_t ecx, uint32_t edx, uint32_t s0);
};
extern "C" void d2_client_82fe0(const D2c82fe0Env* e, uint32_t unit);
