#pragma once
// Retail-layout (1.13c) D2Game+0xCC750 .. 0xCD790: the target and interaction checks of the client packets (merc, party, trade, pickup). Arguments are
// 32-bit retail addresses; the callees trap here and the FN probes install stand-ins through D2Seam. Each row's own file defines its seams.
#include <cstdint>

// 0xCC750 (ECX = game, EAX = unit id, stack = player ref): 0 when the unit is the one the player's list names (and its commands are cleared), else 1.
uint32_t D2Retail113_CC750(uint32_t game, uint32_t unitId, uint32_t param1);
uint32_t D2R_CC750_A4C0(uint32_t param1);                          // Ordinal 10920
void     D2R_CC750_21420();                                         // AIGENERAL_FreeAllAiCommands step
void     D2R_CC750_21290(uint32_t game, uint32_t unit, uint32_t buf);   // EAX = game, ECX = unit, stack buffer
void     D2R_CC750_DFFD0(uint32_t a, uint32_t b);
uint32_t D2R_CC750_Tables();                                        // *sgptDataTables

// 0xCCB10 (this = record, then game, param_2): 1 when the record names another live unit.
uint32_t D2Retail113_CCB10(uint32_t thisp, uint32_t game, uint32_t param2);
uint32_t D2R_CCB10_E03A0(uint32_t game, uint32_t id);

// 0xCCDB0 (ECX = id, EAX = game, stack = record) and 0xCCE20 (ECX = id, EAX = game, EDI = record).
bool     D2Retail113_CCDB0(uint32_t thisId, uint32_t game, uint32_t param1);
uint32_t D2R_CCDB0_AB26(uint32_t a, uint32_t b);
bool     D2Retail113_CCE20(uint32_t id, uint32_t game, uint32_t edi);
uint32_t D2R_CCE20_AB26(uint32_t a, uint32_t b);
uint32_t D2R_CCE20_A6A6(uint32_t a);

// 0xCCFE0 (EAX = mode, ECX = game, EDX = id, stack = unit).
uint32_t D2Retail113_CCFE0(int32_t mode, uint32_t game, uint32_t id, uint32_t unit);
uint32_t D2R_CCFE0_E03A0(uint32_t game, uint32_t id);
uint32_t D2R_CCFE0_E150(uint32_t unit);
uint32_t D2R_CCFE0_A478(uint32_t x);
uint32_t D2R_CCFE0_A556(uint32_t x);
uint32_t D2R_CCFE0_A56E(uint32_t x);
uint32_t D2R_CCFE0_A454(uint32_t x);
uint32_t D2R_CCFE0_A3B8(uint32_t x);
uint32_t D2R_CCFE0_CB690(uint32_t a, uint32_t b);

// 0xCD630 (ECX = packet length, EAX = packet, EBX = out B; stack game, unit, out A); its target check is 0xCCFE0.
int32_t  D2Retail113_CD630(uint32_t len, uint32_t pkt, uint32_t game, uint32_t unit, uint32_t outA, uint32_t outB);

// 0xCD790 (ECX = this, EAX = EDI value, stack param_1, unit).
uint32_t D2Retail113_CD790(uint32_t thisp, uint32_t eax, uint32_t param1, uint32_t unit);
uint32_t D2R_CD790_A478(uint32_t unit);
uint32_t D2R_CD790_1280();
uint32_t D2R_CD790_A68E(uint32_t unit, uint32_t id);
uint32_t D2R_CD790_AB56(uint32_t d);
uint32_t D2R_CD790_AB5C(uint32_t d);
uint32_t D2R_CD790_AB4A(uint32_t d);
void     D2R_CD790_E07A0(uint32_t unit, uint32_t ecx, uint32_t edi);
uint32_t D2R_CD790_A598(uint32_t d);
uint32_t D2R_CD790_AAC6(uint32_t d);
uint32_t D2R_CD790_AAD8(uint32_t d);
void     D2R_CD790_AB08(uint32_t d, uint32_t v);
void     D2R_CD790_AAFC(uint32_t d, uint32_t v);
void     D2R_CD790_AB14(uint32_t d, uint32_t v);
void     D2R_CD790_AB2C(uint32_t d, uint32_t a, uint32_t b);
uint32_t D2R_CD790_AAF0(uint32_t d, uint32_t t, uint32_t z);
uint32_t D2R_CD790_AAAE(uint32_t d);
uint32_t D2R_CD790_AB32(uint32_t d);
void     D2R_CD790_A5FE(uint32_t d, uint32_t v);

#include "../../../d2game/d2game_retail_rw.h"
// the game's unit hash: bucket (id & 0x7f) at base+off, chained through +0xE4, id at +0xC
inline uint32_t d2g_chain(uint32_t base, uint32_t off, uint32_t id) {
	uint32_t n = rw32(base, off + (id & 0x7f) * 4);
	while (n && rw32(n, 0xc) != id) n = rw32(n, 0xe4);
	return n;
}
