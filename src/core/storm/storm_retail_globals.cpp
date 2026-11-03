// src/core/storm/storm_retail_globals.cpp -- definitions for storm_retail_globals.h.
#include "storm_retail_globals.h"

uint32_t g_stormRetailImage[0x6000 / 4];

// State-init mapping: a retail global at rva (0x50000..0x56000) maps to its backing dword; anything else is 0.
extern "C" uint32_t* Storm_RetailGlobalAddr(uint32_t rva) {
    if (rva < 0x50000u || rva >= 0x56000u) return nullptr;
    return &g_stormRetailImage[(rva - 0x50000u) >> 2];
}
