// Raw 32-bit-image inventory placement (1.13c D2Common 0x1E1B0 / 0x208E0 / 0x20A30); tables and allocators are supplied by the caller.
#pragma once
#include <cstdint>
struct InvEnv {
    uint32_t invCount; const uint8_t* inv;       // inventory.txt: record count, rows (0xF0 bytes)
    uint32_t itemCount; const uint8_t* items;    // items.txt: record count, rows (0x1A8 bytes)
    uint32_t (*alloc)(uint32_t size, void* ctx);                  // Fog 10045 stand-in (zeroing alloc)
    uint32_t (*realloc)(uint32_t old, uint32_t size, void* ctx);  // Fog 10047 stand-in
    void* ctx;
};
uint32_t D2Common_1E1B0_GetGrid(const InvEnv& e, uint32_t inv, int32_t index, uint32_t info);
uint32_t D2Common_208E0_GetFreePosition(const InvEnv& e, uint32_t inv, uint32_t item, int32_t rec, uint32_t pX, uint32_t pY, uint8_t page);
uint32_t D2Common_20A30_ReserveSpot(const InvEnv& e, uint32_t inv, uint32_t owner, uint32_t item);
uint32_t D2Common_1F1F0_CanItemBePlaced(const InvEnv& e, uint32_t inv, uint32_t item, int32_t x, int32_t y, int32_t rec, uint32_t pExch, uint32_t pHover, uint8_t page);
bool D2Common_81FB0_GetLayout(const InvEnv& e, int32_t rec, int32_t sub, uint8_t* out24);
uint32_t D2Common_20D20_ReserveAll(const InvEnv& e, uint32_t unit, uint32_t inv, uint32_t owner);
uint32_t D2Common_20B50_CanPlaceSlot(const InvEnv& e, uint32_t inv, uint32_t flag, int32_t index, int32_t rec);
