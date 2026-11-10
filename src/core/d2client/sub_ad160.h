// src/core/d2client/sub_ad160.h -- Env and entry point of sub_ad160.cpp (its FN probe includes it).
#pragma once
#include <cstdint>
namespace d2client_ad160 {
struct Env {
 uint8_t* (*ga)(uint32_t rva);                                   // address of the D2Client global at RVA (the type-2 unit table, 0xBAA08)
 void (*fatal)(int32_t msgId);                                   // Ordinal_10265 + Ordinal_10024 + _exit(-1)
 void (*ord501)(uint8_t* dst, const uint8_t* src, int32_t n);    // 0xBE96, three stack words
 uint8_t* (*f4D630)(uintptr_t pkt, uint32_t edx);                // ecx = pkt (unspecified on one path), edx = u32 at pkt+1
};
void sub_AD160(const Env& e, uintptr_t pkt);
}
