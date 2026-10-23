// 1.13c D2Launch+0x18870 returns 1 unless FUN_6fa51be0 is true, then FUN_6fa58810()
#include <cstdint>
extern "C" { __attribute__((weak)) bool FUN_6fa51be0(); __attribute__((weak)) uint32_t FUN_6fa58810(); }
extern "C" uint32_t FUN_6fa58870() { return FUN_6fa51be0() ? FUN_6fa58810() : 1; }
