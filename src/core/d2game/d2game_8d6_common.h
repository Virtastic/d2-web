// Shared by the one-file-per-function 1.13c D2Game ports in 0x8D600..0x918D0: retail addresses are 32-bit
// integers read at their retail byte offsets (the same convention as d2game_8D860.cpp); callees are seams.
#pragma once
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
namespace d2g8 {
static inline const uint8_t* P(uint32_t a) { return (const uint8_t*)(uintptr_t)a; }
static inline uint8_t* PW(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
static inline uint32_t r32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, P(a + o), 4); return v; }
static inline uint16_t r16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, P(a + o), 2); return v; }
static inline uint8_t r8(uint32_t a, uint32_t o) { return P(a + o)[0]; }
static inline void w32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy(PW(a + o), &v, 4); }
static inline void w16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy(PW(a + o), &v, 2); }
static inline void w8(uint32_t a, uint32_t o, uint8_t v) { PW(a + o)[0] = v; }
[[noreturn]] static inline void fatal(uint32_t) { _exit(-1); }   // retail: #10265(id) -> #10024 -> _exit(-1)
}
