// Env of the D2Client+0xC21F0 port (retail virtual addresses; callees at their retail register/stack conventions, from the disassembly).
#pragma once
#include <cstdint>
namespace d2client_c21f0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 int32_t (*o10048)();                          // thunk 0xD0B4, no arguments
 void (*open)(int32_t eax, int32_t h, P tag);  // 0x3CCC0: EAX = count, stack = (h, tag)
 void (*close)(int32_t eax, int32_t h, P tag); // 0x3CC60: same shape
 void (*call0)(uint32_t rva);                  // argument-less callees (the RVA names the one)
 uint32_t (*callr)(uint32_t rva);              // argument-less callees whose EAX result is read (0x42090, 0x420E0, 0x59830)
 void (*s1)(uint32_t rva, int32_t s0);         // one stack word (0x12DD0, 0x59740)
 void (*f8a4d0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi); // 0x8A4D0: four register arguments
 void (*o10006)();                             // thunk 0xD1B0 (ECX = 0)
};
void sub_C21F0(const Env& e, uint32_t state);
}
