// Callee seams of the 1.13c D2Game+0xC8050 port (a task callback; ECX = game, EDX = event record, plain ret, no result).
// Every convention is the one the disassembly pins (ret N of the callee, the pushes at the call site, the registers set before it).
// The two retail _exit paths (a fatal log line, then _exit(-1)) are the `fatal` seam: a probe aborts there, the engine terminates.
#pragma once
#include <cstdint>
struct GC8050Env {
    uint32_t (*a4c0)(uint32_t unit);                                   // D2Common #10920, stdcall 1; the result points at a table whose +0x10 + n*4 holds the per-class word
    uint32_t (*f1130)(uint32_t ecx);                                   // 0x1130: ECX = unit, plain ret
    uint32_t (*f1100)(uint32_t ecx);                                   // 0x1100: ECX = unit, plain ret
    uint32_t (*a478)(uint32_t unit);                                   // #10331, stdcall 1
    uint32_t (*a808)(uint32_t v);                                      // #10057, stdcall 1
    void     (*dffd0)(uint32_t esi, uint32_t s0, uint32_t s1);         // 0xDFFD0: ESI = unit, stack (0x18, unit), ret 8
    uint32_t (*a56e)(uint32_t v);                                      // #10459, stdcall 1 (only AL is used)
    uint32_t (*a556)(uint32_t v);                                      // #10826, stdcall 1
    uint32_t (*ae98)(uint32_t unit, uint32_t* xy, uint32_t k, uint32_t f1, uint32_t f2, uint32_t z);   // #10654, stdcall 7: (unit, xy, xy, 3, 0x3e01, 0xc01, 0); xy is in/out
    uint32_t (*f1280)(uint32_t eax);                                   // 0x1280: EAX = unit, plain ret
    uint32_t (*e1d90)(uint32_t ecx, uint32_t edx, const uint32_t* w);  // 0xE1D90: ECX = 2, EDX = 0x3b, 7 stack words w[0..6] (ret 0x1C)
    void     (*a4a2)(uint32_t u, uint32_t z);                          // #11090, stdcall 2
    uint32_t (*a22e0)(const uint32_t* w);                              // 0xA22E0: 4 stack words (ret 0x10)
    uint32_t (*aee0)(uint32_t u);                                      // #10258, stdcall 1 (AL used)
    void     (*aec8)(uint32_t u, uint32_t v);                          // #10111, stdcall 2
    void     (*a51a)(uint32_t a, uint32_t b);                          // #10346, stdcall 2
    void     (*f35eb0)(uint32_t eax, uint32_t esi);                    // 0x35EB0: EAX = new state byte, ESI = game, plain ret
    void     (*dfcc0)(uint32_t ebx, uint32_t edi, uint32_t stk);       // 0xDFCC0: EAX = 0, EBX = callback, EDI = owner, 1 stack word (ret 4)
    void     (*f35910)(uint32_t eax, uint32_t ecx, uint32_t stk);      // 0x35910: EAX = rec[2], ECX = rec[3], 1 stack word (ret 4)
    void     (*cbe70)(uint32_t edx, uint32_t game4, uint32_t four);    // 0xCBE70: EDX = log text, 2 stack words (ret 8)
    void     (*f366a0)(uint32_t eax, uint32_t ecx, uint32_t esi);      // 0x366A0: EAX = tag, ECX = rec[0], ESI = rec[3], plain ret
    void     (*f33920)(uint32_t eax, uint32_t a, uint32_t b);          // 0x33920: EAX = game[4], stack (0xE, 0xD) (ret 8)
    void     (*f34c00)(uint32_t eax, uint32_t game4);                  // 0x34C00: EAX = rec[3], 1 stack word (ret 4)
    void     (*f33880)(uint32_t eax, uint32_t esi, uint32_t ebp, uint32_t edi);   // 0x33880: EAX = game+0x1C, ESI = rec[3] or -1, plain ret (EBP = rec[3], EDI = game live)
    uint32_t (*a568)(uint32_t u, uint32_t a, uint32_t b);              // #10174, stdcall 3
    void     (*a57a)(uint32_t u, uint32_t a, uint32_t b);              // #10003, stdcall 3
    void     (*a586)(uint32_t u, uint32_t a, uint32_t b);              // #10593, stdcall 3
    void     (*nodeCall)(uint32_t node, uint32_t fn);                  // call [node+0xF0] with ECX = node
    uint32_t (*logOn)(void);                                           // the dword at D2Game+0x111C20
    uint32_t (*isBadCode)(uint32_t p);                                 // kernel32 IsBadCodePtr
    void     (*fatal)(uint32_t code);                                  // the fatal log line (code = line number) then _exit(-1); never returns
    uint32_t (*fp)(uint32_t rva);                                      // the address of the D2Game function at rva
};
extern "C" void d2game_c8050_port(const GC8050Env* e, uint32_t game, uint32_t rec);
