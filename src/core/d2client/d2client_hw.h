// Shared by the one-file D2Client ports that reach their callees through D2cHost (client_host.h): hc() logs one callee call as the
// capture's `hook` does (rva, eax ecx edx ebx esi, four stack words) and returns the scripted value; rd/wr read and write a retail
// global that sits inside a pinned window (win = the module address the window's first byte stands for).
#pragma once
#include <cstdint>
#include <cstring>
#include "client_host.h"
static inline uint32_t hc(D2cHost h, uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0, uint32_t esi = 0,
                          uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t* out0 = nullptr) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.edx = edx; c.ebx = ebx; c.esi = esi; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    uint32_t r = h(&c); if (out0) *out0 = c.out[0]; return r;
}
struct Win {   // a pinned window of the module image: base pointer + the module address of byte 0
    uint8_t* p; uint32_t a;
    uint32_t rd(uint32_t addr) const { uint32_t v; memcpy(&v, p + (addr - a), 4); return v; }
    void wr(uint32_t addr, uint32_t v) const { memcpy(p + (addr - a), &v, 4); }
    uint8_t rb(uint32_t addr) const { return p[addr - a]; }
    void wb(uint32_t addr, uint8_t v) const { p[addr - a] = v; }
};
static inline uint32_t P32(const void* p) { return (uint32_t)(uintptr_t)p; }
static inline uint32_t ld(const void* p, uint32_t off) { uint32_t v; memcpy(&v, (const uint8_t*)p + off, 4); return v; }
static inline void st(void* p, uint32_t off, uint32_t v) { memcpy((uint8_t*)p + off, &v, 4); }
