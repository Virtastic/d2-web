// Callee seams of the 1.13c D2Game+0x94970 port (fastcall ECX = game, EDX = unit, stack = skill id and a parameter word, ret 8).
#pragma once
#include <cstdint>
struct G94970Env {
    uint32_t sgptCell;                                                              // value of the slot at 0xF829C: address of the cell that holds sgptDataTables
    uint32_t (*f9fa30)(uint32_t unit, int32_t* x, int32_t* y);                      // 0x9FA30: EAX = unit, EBX = &x, stack = &y (ret 4)
    int32_t  (*f1340)(uint32_t skill, uint32_t unit);                               // 0x91340: EAX = skill, stack unit (ret 4)
    int32_t  (*f1240)(uint32_t skill, uint32_t unit, uint32_t p4);                  // 0x91240: EDI = skill, EBX = unit, stack p4 (ret 4)
    int32_t  (*t2ac9a)(uint32_t unit, uint32_t rowWord, uint32_t skill, uint32_t p4);  // 0xAC9A thunk (ret 0x10)
    void     (*t2a628)(uint32_t* seed, uint32_t low);                               // 0xA628 thunk: ECX = &seed, EDX = low word, writes the 8-byte seed
    uint32_t (*t2a478)(uint32_t unit);                                              // 0xA478 thunk (ret 4)
    uint32_t (*f21280)(uint32_t room, int32_t x, int32_t y);                        // 0x1280: EAX = room, EBX = x, EDI = y
    void     (*f8f930)(uint32_t game, const uint32_t* blk);                         // 0x6F930: ECX = game, EDX = 23-dword block
};
extern "C" uint32_t d2game_94970_port(const G94970Env* e, uint32_t game, uint32_t unit, int32_t skill, uint32_t p4);
