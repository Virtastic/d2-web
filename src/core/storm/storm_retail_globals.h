// src/core/storm/storm_retail_globals.h -- backing store for Storm.dll's data-section globals.
// Retail addresses (0x6FC40000..0x6FC46000, module base 0x6FBF0000) are not inside wasm memory, so a port's
// reads and writes there land in g_stormRetailImage. The FN probes map state-init writes through
// Storm_RetailGlobalAddr (rva = retail address - 0x6FBF0000).
#pragma once
#include <cstdint>

extern uint32_t g_stormRetailImage[0x6000 / 4];

inline uint32_t& Storm_RetailW(uint32_t a) { return g_stormRetailImage[(a - 0x6FC40000u) >> 2]; }
inline bool Storm_IsRetailGlobal(uint32_t a) { return a >= 0x6FC40000u && a < 0x6FC46000u; }

extern "C" uint32_t* Storm_RetailGlobalAddr(uint32_t rva);
