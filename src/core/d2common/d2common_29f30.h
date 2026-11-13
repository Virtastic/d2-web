// Env of the 1.13c D2Common+0x29F30 port: retail's callees (0x719A0, 0x9240, 0x24430, 0x38C20, 0x24C30, 0x3A740) as functions.
// Handles are uintptr_t so the same port serves the 32-bit hook replay and the wasm64 build.
#pragma once
#include <cstdint>
struct D2UnitStrc;
struct D2Common29F30Env {
    uint32_t  (*rd32)(uintptr_t);
    uint32_t  (*rd8)(uintptr_t);
    uintptr_t (*itemTxt)(uint32_t classId);        // 0x719A0
    int32_t   (*codeIndex)(uint32_t code);         // 0x9240 (Fog 10213 on the weapons/armor table, flag 0)
    uintptr_t (*weaponArmorRow)(int32_t idx);      // table base + idx * 0x1a8
    uint32_t  (*typeIs)(uintptr_t item, uint32_t type);   // 0x74430
    int32_t   (*getBase)(uintptr_t item, int32_t stat);   // 0x88C20 (flag 0)
    uint32_t  (*isThrowable)(uintptr_t item);      // 0x74C30
    void      (*setStat)(uintptr_t item, int32_t stat, int32_t v);   // 0x8A740 (layer 0)
};
extern "C" void D2Common_29F30_ItemDefenseOrDamage(uintptr_t item, uint32_t stat, const D2Common29F30Env* e);
// Live-graph path: the real callees on a carried unit graph (the vendored routine).
extern "C" void D2Common_29F30_ItemDefenseOrDamageLive(D2UnitStrc* item, int32_t stat);
