// The callee seams of the D2Launch ports d2l_9bf0, d2l_110c0, d2l_a150, d2l_ae60, d2l_d050, d2l_d140, d2l_ea90 and d2l_f5b0
// (one file each, src/core/d2launch/d2launch_<rva>.cpp). Each seam takes retail's register and stack words in order (ECX, EDX,
// then the stack words); module addresses travel as their retail virtual addresses (image base 0x6FA40000).
#pragma once
#include <cstdint>
struct D2lSeams {
    void (*t9a24)(uint32_t s0, uint32_t s1);                          // Fog/D2Win ordinal thunk, stdcall 2
    void (*t97fc)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1); // Fog free (ECX = block, EDX = file, line, 0)
    void (*t996a)(uint32_t ecx);
    void (*t9862)(uint32_t ecx, uint32_t edx);
    void (*t987a)(uint32_t ecx, uint32_t edx);
    uint32_t (*t980e)();
    void (*t9a1e)(uint32_t s0, uint32_t s1, uint32_t s2);
    void (*t98ce)(uint32_t ecx, uint32_t edx, uint32_t s0);
    uint32_t (*t9b2c)();
    void (*cd70)(); void (*a8c0)(); uint32_t (*b600)(); void (*ae60)(); void (*c3f0)(); void (*cc50)(); void (*bda0)();
    void (*d050)(uint32_t eax);
    uint32_t (*e510)(const uint32_t* blk);                             // ECX = the two-word block
    void (*t9790)(uint32_t dst, uint32_t src, uint32_t max);           // stdcall 3
    void (*t9844)(uint32_t p, uint32_t v);                             // stdcall 2
    uint32_t (*t1830)(uint32_t z);                                     // _time32 (cdecl 1)
};
