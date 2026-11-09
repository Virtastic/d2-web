// src/core/d2client/d2client_batch_ad4b0.h -- the Env and entry points of d2client_batch_ad4b0.cpp (its FN probes include it).
#pragma once
#include <cstdint>
namespace d2client_batch_ad4b0 {
struct Env {
 uint8_t* (*ga)(uint32_t rva);                    // address of D2Client global at RVA
 void (*fatal)(int32_t msgId);                    // Ordinal_10265 + Ordinal_10024 + _exit(-1)
 void (*f62E60)(uint32_t a);
 void (*f4C3C0)(uintptr_t pkt,const void* copy7);
 void (*f7DFB0)(const void* copy40);
 uintptr_t (*f4D630)(uintptr_t pkt,uint32_t id);
 void (*f4DA70)(uint32_t v);
 void (*f272E0)();
 void (*f1A1B0)(uintptr_t a,uint32_t b,uint32_t c);   // FUN_6fada1b0
 void (*f194E0)(const void* copy10);              // EAX = &10-byte packet copy
 // AE080 callees: the local packet copy is EAX (34BB0 34AF0 34990 34750 32320 33AD0 339E0), ESI (35AA0 32E10), EDI (34080) or a stack word (36810 32C60)
 void (*f34BB0)(const void* eax); void (*f34AF0)(const void* eax); void (*f34990)(const void* eax); void (*f36810)(const void* pkt);
 void (*f34750)(const void* eax); void (*f32320)(const void* eax); void (*f34080)(const void* edi); void (*f35AA0)(const void* esi); void (*f33AD0)(const void* eax);
 void (*f32C60)(const void* pkt); void (*f32E10)(const void* esi); void (*f339E0)(const void* eax);
 uintptr_t (*f54E20)(uint32_t key,uintptr_t row,uint32_t idx);
 void (*f32FE0)(uintptr_t unit);
 void (*f21D60)(uintptr_t a,uint32_t b,uint32_t c);
 void (*fAC240)(const void* eax_pkt,void* esi_dst);  // D2Client+0xAC240: EAX = packet, ESI = 16-byte local it fills (b0, w1, w3, w5, b7, b8, d9, b13, b14)
 void (*fAC1B0)(const void* eax_pkt,void* esi_dst);  // D2Client+0xAC1B0: EAX = packet, ESI = 16-byte local it fills (b0, w1, w3, w5, w7, w9, b11, b12)
 void (*f2A1B0)(uintptr_t a,uint32_t b,uint32_t c,uint32_t d); // D2Client+0x2A1B0, four stack words (ret 0x10)
 void (*o10887)(uintptr_t a,uint32_t b,uint32_t c,uint32_t d);
 void (*f6A6C0)(uintptr_t esi); void (*f56210)(uintptr_t esi);   // D2Client+0x2A6C0 (ESI = unit, EDI = 5) and +0xA6210 (ESI = unit)
};
void sub_AD4B0(Env& e,uint32_t a);
void sub_ADEF0(Env& e,uintptr_t p);
void sub_AEBE0(Env& e,const uint8_t* pkt);
void sub_AECA0(Env& e,const uint8_t* pkt);   // d2client_aeca0.cpp
void sub_AE080(Env& e,const uint8_t* src);
} // namespace d2client_batch_ad4b0
