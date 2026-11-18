#pragma once
// 1.13c D2Game+0x2C2D0 (new empty game) in retail layout: arguments and globals are the 32-bit retail addresses of the game, client and
// callback blobs (the game's own structs are 64-bit and the layouts differ), so the game has no caller for it. Every callee is reached
// through the D2_CALL stub D2Retail113_GameCall (keyed by RVA; registers EAX, ECX, EDX, EBX, ESI, then four stack words), which traps in
// the game; the FN probe installs the hook-log stand-ins through D2Retail113_GameSetHooks.
#include <cstdint>

uint32_t D2Retail113_GameCall(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t s0, uint32_t s1,
                              uint32_t s2, uint32_t s3);
// REGS `J`: EDI handed to the callee just called
void D2Retail113_GameEdi(uint32_t rva, uint32_t edi);
void D2Retail113_GameSetHooks(uint32_t (*call)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t),
                              void (*edi)(uint32_t, uint32_t));
// pseudo-RVA of the one indirect call: the callback table's cookie function, handed in s0
constexpr uint32_t kD2Retail113CookieCall = 0x7FFFFFFFu;
// flags = EAX, a = the 11 stack words; srvCell / cbCell / diffCell the module words 0x6FD31C24, 0x6FD31C3C, 0x6FD31C30
void D2Retail113_2C2D0(uint32_t flags, const uint32_t* a, uint32_t srvCell, uint32_t cbCell, uint32_t diffCell);
