// Env of the D2Client+0xC1C40 port (retail virtual addresses; callees at their retail register/stack conventions, from the disassembly).
#pragma once
#include <cstdint>
namespace d2client_c1c40 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t); void (*wr8)(P, uint8_t);
 int32_t (*o10048)();                         // thunk 0xD0B4, no arguments
 void (*open)(int32_t eax, int32_t h, P tag);  // 0x3CCC0: EAX = count, stack = (h, tag)
 void (*close)(int32_t eax, int32_t h, P tag); // 0x3CC60: same shape
 void (*call0)(uint32_t rva);                  // argument-less callees (the RVA names the one)
 void (*f12dd0)(int32_t s0);                   // 0x12DD0 (push 1)
 void (*fbfa10)(P edi);                        // 0xBFA10: EDI = a table address
};
void sub_C1C40(const Env& e, uint32_t state);
}
