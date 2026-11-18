// Retail D2Game's trivial thunks in the 0x2A470-0x2A560 table: static-block address getters, constant-0 returns and empty bodies.
// No 1.10f D2MOO counterpart; each is taken from the 1.13c disassembly. Offsets are module-relative image offsets.
#include <cstdint>

// 1.13c D2Game+0x2A470 -- returns the address of the module's static block at image offset 0xFA968.
uint32_t d2game_2a470() { return 0xFA968u; }
// 1.13c D2Game+0x2A480 -- returns the address of the module's static block at image offset 0xFA950.
uint32_t d2game_2a480() { return 0xFA950u; }
// 1.13c D2Game+0x2A4A0 -- returns 0.
uint32_t d2game_2a4a0() { return 0; }
// 1.13c D2Game+0x2A4B0 -- returns 0.
uint32_t d2game_2a4b0() { return 0; }
// 1.13c D2Game+0x2A4C0 -- returns 0.
uint32_t d2game_2a4c0() { return 0; }
// 1.13c D2Game+0x2A4D0 -- empty body (stdcall, 4 stack words dropped).
void d2game_2a4d0(uint32_t, uint32_t, uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A4E0 -- empty body (stdcall, 2 stack words dropped).
void d2game_2a4e0(uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A4F0 -- returns the module static block at image offset 0xFA898.
uint32_t d2game_2a4f0() { return 0xFA898u; }
// 1.13c D2Game+0x2A500 -- empty body.
void d2game_2a500(uint32_t) {}
// 1.13c D2Game+0x2A510 -- returns 0.
uint32_t d2game_2a510() { return 0u; }
// 1.13c D2Game+0x2A520 -- empty body.
void d2game_2a520() {}
// 1.13c D2Game+0x2A530 -- empty body.
void d2game_2a530(uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A540 -- empty body, drops 5 stack arguments (20 bytes popped).
void d2game_2a540(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A550 -- empty body.
void d2game_2a550(uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A560 -- empty body.
void d2game_2a560(uint32_t) {}
