// src/core/d2win/d2win_open_util.h -- helpers shared by the one-file-per-function D2Win 1.13c
// ports (d2win_<rva>.cpp). Retail controls are 32-bit structures: fields are read and written at
// the retail byte offsets, and a 32-bit pointer slot is zero-extended, never sign-extended.
// Every null or type check that retail answers with Fog #10265(id) -> Fog #10024("", msg) ->
// _exit(-1) is d2win_open_fatal here.
#pragma once
#include <cstdint>
#include <cstring>
#include <unistd.h>

namespace d2win_open {

[[noreturn]] static inline void fatal(uint32_t) { _exit(-1); }

static inline uint8_t  rd8(const void* p, uint32_t o)  { uint8_t v;  std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v)   { std::memcpy((uint8_t*)p + o, &v, 1); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// A 32-bit retail pointer slot as a host pointer (zero-extended) and back.
static inline uint8_t* ptr(uint32_t v) { return (uint8_t*)(uintptr_t)v; }
static inline uint32_t slot(const void* p) { return (uint32_t)(uintptr_t)p; }

// gdwBitMasks[n] == 1 << n (Fog's table, imported through 0x6F8FB17C).
static const uint32_t kBm0 = 1u << 0, kBm2 = 1u << 2, kBm3 = 1u << 3;

// D2Lang Unicode::strlen / Unicode::strcpy on UTF-16 code units.
static inline int32_t u16len(const uint16_t* s) { int32_t n = 0; while (s[n] != 0) ++n; return n; }
static inline void u16cpy(uint16_t* d, const uint16_t* s) { do { *d++ = *s; } while (*s++ != 0); }

// The current font's pixel height: font table 0x6F901518 (5 dwords per entry, entry[0] -> font),
// indexed by the current font 0x6F8FFD34; the height is the byte at font+0xA.
static inline uint32_t font_height(const uint32_t* fontTable, uint32_t fontIndex) {
    return rd8(ptr(fontTable[fontIndex * 5]), 0xA);
}

}  // namespace d2win_open
