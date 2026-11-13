// Callee seams of the 1.13c D2Game+0x76270 port (event handler for a unit's removal; ECX = game, EDX = event record, plain ret).
// Every callee's register and stack convention is the one the disassembly pins (ret N of the callee, pushes at the call site).
#pragma once
#include <cstdint>
struct G76270Env {
    void     (*fDA750)(uint32_t unit, uint32_t game);                      // 0xDA750: EDI = unit, stack game (ret 4)
    void     (*t2A4A2)(uint32_t unit, uint32_t z);                         // 0xA4A2, D2Common #11090, stdcall 2 words
    void     (*f75350)(uint32_t unit, uint32_t game, uint32_t flag);       // 0x75350: EAX = unit, ECX = game, stack dword (the byte at record+0x14, zero-extended; ret 4)
    void     (*f75900)(uint32_t rec, uint32_t game);                       // 0x75900: EDI = record, stack game (ret 4)
    uint32_t (*t2A514)(uint32_t unit);                                     // 0xA514, #10830, 1 word
    uint32_t (*t2A478)(uint32_t unit);                                     // 0xA478, #10331, 1 word (the caller's second pushed word stays for 0xB072)
    void     (*t2B072)(uint32_t a, uint32_t id);                           // 0xB072, #10873, 2 words: the 0xA478 result, then the dead pushed word (unit+0xC or -1)
    uint32_t (*t2A454)(uint32_t p);                                        // 0xA454, #10867, 1 word
    uint32_t (*t2A3B8)(uint32_t p);                                        // 0xA3B8, #10750, 1 word
    void     (*f755c0)(uint32_t unit, uint32_t game);                      // 0x755C0: EBX = unit, stack game (ret 4)
    void     (*f760f0)(uint32_t unit, uint32_t game);                      // 0x760F0: EAX = unit, stack game (ret 4)
    uint32_t (*fA28F0)(uint32_t game, uint32_t sel, const uint32_t* a);    // 0xA28F0: ECX = game, EDX = sel, 8 stack words a[0..7] (ret 0x20); returns the new unit
    uint32_t (*t2A7BA)(uint32_t b);                                        // 0xA7BA, #10237, 1 word
    uint32_t (*t2A42A)(uint32_t unit, uint32_t a, uint32_t b);             // 0xA42A, #10973, 3 words (ret 0xC)
    void     (*t2AD7E)(uint32_t cls, uint32_t w74, uint32_t b6d, uint32_t r, uint32_t one, uint32_t* out);   // 0xAD7E, #11089, 6 words (ret 0x18); fills out[0..13]
    int32_t  (*f14D0)(int32_t eax, int32_t ecx, int32_t edx);              // 0x14D0: EAX, ECX, EDX in, plain ret
    uint32_t (*f11D0)(uint32_t seed, int32_t range);                       // 0x11D0: ECX = seed pointer, ESI = range, plain ret
    void     (*fDE380)(const uint32_t* a);                                 // 0xDE380: 10 stack words a[0..9] (ret 0x28)
    uint32_t (*dataTables)(void);                                          // the sgptDataTables pointee (D2Game+0xF829C, two derefs)
    uint32_t (*bitMasks)(void);                                            // the dword the pointer at D2Game+0xF8BE0 points at, +0x10
};
extern "C" uint32_t d2game_76270_port(const G76270Env* e, uint32_t game, uint32_t rec);
