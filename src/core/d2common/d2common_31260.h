#pragma once
#include <cstdint>
// D2Common 0x31260 seams (retail callees, all stdcall one word): 0x1E1B0 (EAX = 0, the module body-location table
// 0x9D378; returns a record whose +0xC points at the equipped-slot block), 0x22E70 (item class: two-handed test),
// 0x30530 (unit: can dual-wield), 0x20320 (inventory: the item it designates), 0x719A0 (item class: items.txt row).
struct D2Common31260Env {
    uint32_t (*slots)();
    uint32_t (*twoHanded)(uint32_t classId);
    uint32_t (*dualWield)(uint32_t unit);
    uint32_t (*invItem)(uint32_t inv);
    uint32_t (*itemsRow)(uint32_t classId);
};
extern "C" uint32_t D2Common_31260_ItemTypeClass(uint32_t unit, uint32_t item, const D2Common31260Env* env);
