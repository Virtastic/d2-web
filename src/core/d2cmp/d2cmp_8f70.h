// src/core/d2cmp/d2cmp_8f70.h -- 1.13c D2CMP+0x8F70: the callee seams and the port's signature.
#pragma once
#include <cstdint>

struct D2Cmp8f70Ops {
    int32_t (*storm10106)(void* handle, uint32_t a0, uint32_t a1, uint32_t a2);   // 0x8E42, ecx = handle, edx = a0, stack a1 a2; -1 = failure
    void (*ord276)(void* handle, uint8_t* buf, uint32_t n);                       // 0x8E24, stack: handle, buf, 0x104 (stdcall)
    uint32_t (*ord10265)(uint32_t code);                                          // 0x8D90, cdecl, code = 0x70 or 0x79; returns eax
    void (*ord10024)(const char* msg, uint32_t v, uint32_t code);                 // 0x8D8A, cdecl, msg = 0x6FE2D30F
    void (*exit_)(int code);                                                      // 0x28D0, _exit(-1), stdcall-shaped; does not return in retail
};

// Arguments as retail pushes them: esi = handle, then the three stack words a0 a1 a2 (ret 0xC).
extern "C" void d2cmp_8f70(void* esi, uint32_t a0, uint32_t a1, uint32_t a2, const char* msg, const D2Cmp8f70Ops& ops);
