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
    P (*f1c90)(P,P,P); I (*r10123)(P); I (*r10331)(P,I,I,I,I); I (*r10851)(I); // B1C90..
    U (*f1560)(I); void (*f1b10)(P,P);       // 1560, F1B10
    void (*f102f0)(P); void (*f10640)(P,P);  // F02F0, F0640
    void (*fatal)(I);                        // A2DA/A2C2/2A3C -> _exit path
};
void sub_F2F00(Env& e,P target,P unit,P game,I flag);
void sub_F2F30(Env& e,P target,P unit,P game,I flag);
void sub_F3000(Env& e,P target,P unit,P game,I w);
}
