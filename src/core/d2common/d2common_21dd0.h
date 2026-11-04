// D2Common 0x21DD0: place an item on an inventory page (stdcall, ret 0x1C).
#pragma once
#include <cstdint>
struct D2InventoryStrc;
struct D2UnitStrc;
extern "C" void D2Common_21DD0_PlaceAtPage(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int32_t nXPos, int32_t nYPos,
                                           int32_t nRecordId, int32_t bUnused, uint8_t nPage);
