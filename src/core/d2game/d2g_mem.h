// Retail-layout field access for the d2g_<rva> ports: structs are read at their 1.13c byte offsets, a pointer field is
// the 32-bit dword retail stores there (the FN blobs live below 4 GiB, so the dword is the address).
#pragma once
#include <cstdint>
#include <cstring>
namespace d2g {
using U = uint32_t; using I = int32_t; using P = void*;
inline U rd(const void* p, U o) { U v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline I rds(const void* p, U o) { return (I)rd(p, o); }
inline uint16_t rd16(const void* p, U o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline uint8_t rd8(const void* p, U o) { return ((const uint8_t*)p)[o]; }
inline void wr(void* p, U o, U v) { std::memcpy((uint8_t*)p + o, &v, 4); }
inline void wr16(void* p, U o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
inline void wr8(void* p, U o, uint8_t v) { ((uint8_t*)p)[o] = v; }
inline P pf(const void* p, U o) { return (P)(uintptr_t)rd(p, o); }
inline void wp(void* p, U o, const void* v) { wr(p, o, (U)(uintptr_t)v); }
inline P at(const void* p, U o) { return (P)((const uint8_t*)p + o); }
inline U W(const void* p) { return (U)(uintptr_t)p; }
}
