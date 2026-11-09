// Env of the D2Client+0xC2790 port (retail virtual addresses; callees at their retail register/stack conventions, from the disassembly).
#pragma once
#include <cstdint>
namespace d2client_c2790 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 void (*fatal)(uint32_t code);                 // 0xBF92 (stack = code), then 0xBF80 and exit(-1) at 0x3664: not returning in retail
 uint32_t (*f_bf8c)();                         // 0xBF8C, no arguments, EAX result
 uint32_t (*f_71a80)(uint32_t eax);            // 0xC1A80, EAX = mode
 uint32_t (*f_a5440)();                        // 0xA5440, no arguments, EAX result
 void (*f_c1c40)(uint32_t eax);                // 0xC1C40, EAX = panel
 void (*f_c21f0)(uint32_t eax);                // 0xC21F0, EAX = panel
 void (*f_6eb50)();                            // 0x6EB50
 void (*f_11f80)();                            // 0x11F80
 void (*f_8bbf0)();                            // 0x8BBF0
 void (*f_d1b0)(int32_t ecx);                  // 0xD1B0 thunk, ECX = x offset
 void (*f_8a4d0)(uint32_t eax, int32_t ecx, uint32_t edx, uint32_t esi, int32_t s0); // 0x8A4D0, four registers + one stack word
 void (*f_161b0)(uint32_t edi, int32_t ebx);   // 0x161B0, EDI and EBX
};
uint32_t sub_C2790(const Env& e, uint32_t panel, uint32_t mode, int32_t a3);
}
