// 1.13c D2Launch+0x18870 (stdcall(p): EAX = p into 0x11BE0, then 0x18810(p); core in d2launch_18870.cpp)
#include <cstdint>
extern "C" {
__attribute__((weak)) bool FUN_6fa51be0(uint32_t p);
__attribute__((weak)) uint32_t FUN_6fa58810(uint32_t p);
uint32_t d2l_18870_core(uint32_t, int (*)(uint32_t), uint32_t (*)(uint32_t));
static int d2l_18870_gate(uint32_t p) { return FUN_6fa51be0(p); }
uint32_t FUN_6fa58870(uint32_t p) { return d2l_18870_core(p, d2l_18870_gate, FUN_6fa58810); }
}
