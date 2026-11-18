#pragma once
// Retail-layout (1.13c) item quality setters and item-drop builders of D2Game.dll. Arguments are the 32-bit retail addresses of
// the item, game, room and drop-block words (the game's own structs are 64-bit and differ); the drop request is the 0x84-byte
// retail block. The callees are D2Game helpers and D2Common ordinals that the game reaches through its own native types, so
// their definitions here trap and the proofs install stand-ins through D2Seam.
#include <cstdint>

// quality family: pP = address of the item word, pQ = address of the caller's low-seed word, drop = the item-drop record
void D2Retail113_101E0(uint32_t pP, uint32_t pQ, uint32_t drop);   // superior (3)
void D2Retail113_104F0(uint32_t pP, uint32_t pQ, uint32_t drop);   // magic (4)
void D2Retail113_10730(uint32_t pP, uint32_t pQ, uint32_t drop);   // rare (6)
void D2Retail113_109B0(uint32_t pP, uint32_t nQ, uint32_t drop, uint32_t nArg);   // normal (2), Q by value
// item teardown, first phase
void D2Retail113_10700(uint32_t game, uint32_t item);
// drop builders (each returns what the item creator 0x11490 returns, or 0 when nothing is dropped)
int32_t D2Retail113_11880(uint32_t unit, uint32_t game, int32_t nIndex, uint32_t a3, uint32_t a4, uint32_t nFlags);
int32_t D2Retail113_11CA0(uint32_t xy, uint32_t game, uint32_t room, uint32_t p2, uint32_t p3, uint32_t p4);
int32_t D2Retail113_11E20(uint32_t game, uint32_t room, uint32_t xy, uint32_t p3, uint32_t p4, uint32_t p5);
int32_t D2Retail113_11FB0(uint32_t xy, uint32_t room, uint32_t game, uint32_t p1, uint32_t p2, uint32_t p3);
int32_t D2Retail113_12230(uint32_t xy, uint32_t game, uint32_t room, uint32_t out);

// quality callees (D2Game helpers named by RVA, D2Common by ordinal)
void D2Retail113_E210();
void D2Retail113_A5B0(uint32_t p, uint32_t q);
void D2Retail113_A628(uint32_t ecx, uint32_t edx);
void D2Retail113_EE90(uint32_t p, uint32_t drop);
void D2Retail113_B090(uint32_t p, uint32_t state);
uint32_t D2Retail113_AFB2(uint32_t ecx);
uint32_t D2Retail113_B378(uint32_t p);
void D2Retail113_14F20(uint32_t drop);
void D2Retail113_15240(uint32_t drop);
uint32_t D2Retail113_103C0(uint32_t pP, uint32_t drop);
void D2Retail113_10600(uint32_t drop, uint32_t p, uint32_t nArg);
void D2Retail113_10550(uint32_t p, uint32_t drop, uint32_t nArg);
// 10700 callees
int32_t D2Retail113_Ord10757_IsSocketable(uint32_t item);
void D2Retail113_E1110(uint32_t game, uint32_t item);
void D2Retail113_Ord10820_FreeInventory(uint32_t inv);
void D2Retail113_8E880(uint32_t game, uint32_t item);
// drop callees
void D2Retail113_Ord10141_UnitCoords(uint32_t unit, uint32_t outXY);
uint32_t D2Retail113_Ord10331_UnitRoom(uint32_t unit);
uint32_t D2Retail113_E930(uint32_t room, uint32_t outXY, int32_t one, uint32_t inXY);
uint32_t D2Retail113_E5D0(uint32_t unit, int32_t zero);
int32_t D2Retail113_11490(uint32_t game, uint32_t drop, int32_t zero);
uint32_t D2Retail113_Ord10630_RoomLevel(uint32_t room);
int32_t D2Retail113_A808(uint32_t room);
int32_t D2Retail113_ADA8(uint32_t level, uint32_t difficulty, uint32_t seedWord);
int32_t D2Retail113_F780(uint32_t game, uint32_t field, int32_t ilvl, uint32_t p2, uint32_t p3, int32_t flag);
uint32_t D2Retail113_1280(uint32_t room, uint32_t x, uint32_t y);
uint32_t D2Retail113_AE98(uint32_t room, uint32_t inXY, uint32_t outXY, int32_t a, int32_t b, int32_t c, int32_t d);
int32_t D2Retail113_F8F0(uint32_t p3, uint32_t game, uint32_t field, int32_t ilvl, uint32_t p4);
int32_t D2Retail113_B1B6(int32_t index);
int32_t D2Retail113_FA30(uint32_t p1, uint32_t game, uint32_t field, int32_t ilvl, uint32_t p2);
int32_t D2Retail113_FFC0(int32_t ilvl, uint32_t game, int32_t minusOne, int32_t z0, int32_t z1);
