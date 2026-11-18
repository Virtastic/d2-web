#pragma once
// Retail-layout (1.13c) D2Game+0xD2F00 (ItemFamilyValidate): arguments are 32-bit retail addresses; the callees trap here and the FN probe
// installs stand-ins through D2Seam. D2Retail113_D2070 is called through D2Seam too.
#include <cstdint>
#include "d2game_d2070.h"
// EDI = player, stack = game, item (ret 8). 1 when the player may take/equip the item: no carry-1 unique or same-group quest item already
// held (own inventory or any corpse's), and the quest-state gates for the item's quest byte pass.
uint32_t D2Retail113_D2F00(uint32_t player, uint32_t game, uint32_t item);
uint32_t D2R_D2F00_FirstItem(uint32_t inv);               // #10460 (0xA3DC)
uint32_t D2R_D2F00_TxtRow(uint32_t classId);              // #10695 (0xA53E)
uint32_t D2R_D2F00_Quality(uint32_t item);                // #10106 (0xA898)
uint32_t D2R_D2F00_FileIndex(uint32_t item);              // #10620 (0xA874)
uint32_t D2R_D2F00_UniqueRec(uint32_t index);             // 0x16F0 (unique-items record for index)
uint32_t D2R_D2F00_Mask8();                               // (byte)gdwBitMasks[8]
uint32_t D2R_D2F00_PlayerData(uint32_t player);           // #10920 (0xA4C0)
uint32_t D2R_D2F00_QuestRec(uint32_t player, uint32_t game); // 0x1020
uint32_t D2R_D2F00_QuestState(uint32_t rec, uint32_t quest, uint32_t flag); // #10174 (0xA568)
uint32_t D2R_D2F00_FirstCorpse(uint32_t inv);             // #10284 (0xA466)
uint32_t D2R_D2F00_CorpseId(uint32_t corpse);             // #10155 (0xA4AE)
uint32_t D2R_D2F00_NextCorpse(uint32_t corpse);           // #10348 (0xA484)
void     D2R_D2F00_Fatal(uint32_t line);                  // #10265/#10024 then _exit(-1) (0x2D3)
