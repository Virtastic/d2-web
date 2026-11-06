// D2Common 1.13c 0x5E870 (bin-table unload driver): the module slots it reads or clears and the seam its callees go through.
// X(field, rva): the 4-byte module globals in the order the function first touches them; rva is relative to the module base.
#pragma once
#include <cstdint>

#define D2C5E870_SLOTS(X) \
    X(flag,0x99e20) X(s_a9c,0xa0a9c) X(s_a90,0xa0a90) X(s_a94,0xa0a94) X(s_ac0,0xa0ac0) X(s_ac4,0xa0ac4) X(s_f7c,0x9ff7c) \
    X(s_f80,0x9ff80) X(s_bbc,0x9fbbc) X(s_be0,0x9fbe0) X(s_af8,0xa0af8) X(s_afc,0xa0afc) X(s_b00,0xa0b00) X(s_d94,0x9fd94) \
    X(s_d9c,0x9fd9c) X(s_bd0,0x9fbd0) X(s_da4,0x9fda4) X(s_d8c,0x9fd8c) X(s_bf4,0x9fbf4) X(s_bf0,0x9fbf0) X(s_b50,0xa0b50) \
    X(s_b4c,0xa0b4c) X(s_b38,0xa0b38) X(s_1420,0xa1420) X(s_141c,0xa141c) X(s_142c,0xa142c) X(s_1434,0xa1434) X(s_1448,0xa1448) \
    X(s_1454,0xa1454) X(s_b94,0xa0b94) X(s_b98,0xa0b98) X(s_b9c,0xa0b9c) X(s_ba0,0xa0ba0) X(s_b8c,0xa0b8c) X(s_b90,0xa0b90) \
    X(s_fa5c,0x9fa5c) X(s_bd8,0x9fbd8) X(s_f88,0x9ff88) X(s_1404,0xa1404) X(s_140c,0xa140c) X(s_1408,0xa1408) X(s_1410,0xa1410) \
    X(s_1414,0xa1414) X(s_1418,0xa1418) X(s_13dc,0xa13dc) X(s_b60,0xa0b60) X(s_1400,0xa1400) X(s_1464,0xa1464) X(s_13f8,0xa13f8) \
    X(s_b54,0xa0b54)

struct D2Common5E870Slots {
#define D2C5E870_F(n, r) uint32_t n;
    D2C5E870_SLOTS(D2C5E870_F)
#undef D2C5E870_F
};

// File-name strings (module RVAs) the guarded frees report with.
enum : uint32_t { D2C5E870_STR_FIELD = 0x8ad20, D2C5E870_STR_ITEM = 0x8eb70, D2C5E870_STR_LVL = 0x8fa68, D2C5E870_STR_DATA = 0x91828, D2C5E870_STR_ANIM = 0x934d4 };

struct D2Common5E870Env {
    void (*call)(uint32_t rva);                                                   // the fourteen void(void) cdecl callees, named by retail RVA
    void (*free9228)(uint32_t block, uint32_t strRva, uint32_t line);             // thunk 0x9228: fastcall(ecx=0, edx=block), stack {file, line, 0}, ret 12
    void (*free9246)(uint32_t block);                                             // thunk 0x9246 (Ordinal 10212): stdcall(block), ret 4
    void (*anim922e)(uint32_t ecx, uint32_t strRva, uint32_t a, uint32_t b);      // thunk 0x922E (Ordinal 10043): fastcall(ecx=*[slot], edx=string), stack {0xbb, 0}, ret 8
    uint32_t (*rd32)(uint32_t p);                                                 // dword read through a slot pointer (the AnimTbls block head)
};

extern "C" void d2_seam_d2common_5e870(D2Common5E870Slots* s, const D2Common5E870Env* e);
