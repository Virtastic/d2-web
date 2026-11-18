#pragma once
// Raw reads and writes at a 32-bit retail address + byte offset, shared by the retail-layout D2Game bodies (D2Retail113_*).
#include <cstdint>
#include <cstring>
inline uint32_t rw32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
inline void     ww32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }
inline uint16_t rw16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
inline void     ww16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 2); }
