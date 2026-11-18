#pragma once
// Retail-layout (1.13c) D2Game+0xD2070 (item-carry conflict walk): arguments are 32-bit retail addresses; the callees trap here and an FN
// probe installs stand-ins through D2Seam.
#include <cstdint>
// EAX = list head (first item of the inventory being searched), EBX = items.txt row of `item`, stack = item. Returns 1 when another item
// on the list from `head` (up to `item` itself) is the same carry-1 unique, or is a quest item of the same group whose code is the same or
// an allowed sibling (ret 4).
uint32_t D2Retail113_D2070(uint32_t head, uint32_t row, uint32_t item);
uint32_t D2R_D2070_Quality(uint32_t item);        // #10106 (0xA898)
uint32_t D2R_D2070_FileIndex(uint32_t item);      // #10620 (0xA874)
uint32_t D2R_D2070_UniqueRow(uint32_t index);     // sgptDataTables unique-items row for index (0xC24 table, 0x14C stride, 0xC28 count), or 0
uint32_t D2R_D2070_Mask8();                       // (byte)gdwBitMasks[8]
uint32_t D2R_D2070_IsItem(uint32_t node);         // #11147 (0xA4CC)
uint32_t D2R_D2070_Next(uint32_t node);           // #10464 (0xA46C)
uint32_t D2R_D2070_InvPage(uint32_t item);        // #10020 (0xA53E side: invpage byte)
uint32_t D2R_D2070_TxtRow(uint32_t classId);      // #10695 (0xA6C4)
