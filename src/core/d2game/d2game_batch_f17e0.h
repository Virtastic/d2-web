// src/core/d2game/d2game_batch_f17e0.h -- the Env seam of d2game_batch_f17e0.cpp, shared with the walk-family probes
// (src/core/fn/d2game_f2f00.cpp, d2game_f2f30.cpp, d2game_f3000.cpp), which build an Env of their own.
#pragma once
#include <cstdint>
namespace d2game_batchf17e0 {
using U = uint32_t; using I = int32_t; using P = void*;
struct Env {
    P (*ptr)(P,U);                          // retail pointer field -> native object
    I (*walk)(P game,P unit,P target,I mode,I x,I y,I flags,I w6); // F2540 (w6 = sixth stack word)
    void (*idle)(P game,P unit,I frames);   // F22C0
    I (*stateTest)(P,I);                    // D2Common 10494
    I (*f114f0)(P,P,P,P,P,I);               // F14F0
    void (*f7a4c0)(P,I);                    // 7A4C0
    I (*pathX)(P); I (*pathY)(P);           // 10867 / 10750
    void (*r11103)(P); void (*change)(P,P,I); // 11103, 75E10 (info built by caller)
    void (*r11005)(P,I); void (*f75070)(P);   // 11005, 75070
    P (*f1c90)(P game,P unit,P a,P b,I x0);  // B1C90: EAX game, stack (unit, a, b, x0)
    I (*r10123)(P); I (*r10331)(P,I,I,I,I); I (*r10851)(I); // AD00, A478, AC6A
    U (*f1560)(I eax,I edx);                 // 1560: EAX class id, EDX 2
    void (*f1b10)(U eax,P game,P unit);      // F1B10: AL 5 over the 1560 result, stack (game, unit)
    void (*f102f0)(P ecx,I eax,I esi,P ebx); // F02F0: ECX unit, EAX/ESI the caller's EAX, stack EBX (ret 4)
    I (*f10640)(P ecx,I eax,I esi,P ebx,I flag); // F0640: ECX unit, EAX 8, ESI the caller's EAX, stack (EBX, flag) (ret 8)
    void (*fatal)(I);                        // A2DA/A2C2/2A3C -> _exit path
};
void sub_F2F00(Env& e,P target,P unit,P game,I flag);
void sub_F2F30(Env& e,P target,P unit,P game,I flag);
void sub_F3000(Env& e,P target,P unit,P game,I w);
P sub_F3220(Env& e,P unit,P a,P game,P b,I x0);
I sub_F41C0(Env& e,P unit,P ebx,I eax);
I sub_F4210(Env& e,P unit,P ebx,I eax);
}
