#pragma once
// Retail-layout (1.13c) NPC routines of D2Game.dll. Arguments are the 32-bit retail addresses of the game, unit and record blobs (the
// game's own structs are 64-bit, the retail layouts differ), so the game has no caller for them; the callees are reached through
// D2_CALL stubs that trap in the game, and the FN probes install the hook-log stand-ins through D2Seam.
// 1.13c D2Game+0xC0B90 (hireling/summon link teardown).
#include <cstdint>

// One generic stub keyed by the callee's RVA (registers EAX, ECX, EDX, EBX, ESI, then the stack words), and the EDI-as-argument record.
uint32_t D2Retail113_NpcCall(uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s, uint32_t s0, uint32_t s1,
                             uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7);
void D2Retail113_NpcEdi(uint32_t rva, uint32_t edi);
// Installs (or, with nullptrs, removes) the stand-ins; the probe TUs hold a different D2Seam, so they go through this.
void D2Retail113_NpcSetHooks(uint32_t (*call)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                                              uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t),
                             void (*edi)(uint32_t, uint32_t));
// dpp = the cell the module global 0x6fd1829c points at (it holds the data-table pointer).
void D2Retail113_C0B90(uint32_t unit, uint32_t game, uint32_t rec, uint32_t dpp);
// 1.13c D2Game+0xC0DB0 (resurrect mercenary), +0xC1250 (create mercenary), +0xBDB10 (Cain identify all).
void D2Retail113_C0DB0(uint32_t guid, uint32_t game, uint32_t player);
uint32_t D2Retail113_C1250(uint32_t game, uint32_t player, uint32_t name, uint32_t seed, uint32_t ver, uint32_t cls, uint32_t dead,
                           uint32_t dtCell, uint32_t bm);
uint32_t D2Retail113_BDB10(uint32_t player, uint32_t guid, uint32_t game);
