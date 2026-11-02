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
 void (*f34BB0)(); void (*f34AF0)(); void (*f34990)(); void (*f36810)(const void* pkt);
 void (*f34750)(); void (*f32320)(); void (*f34080)(); void (*f35AA0)(); void (*f33AD0)();
 void (*f32C60)(const void* pkt); void (*f32E10)(); void (*f339E0)();
 uintptr_t (*f54E20)(uint32_t key,uintptr_t row,uint32_t idx);
 void (*f32FE0)(uintptr_t unit);
 void (*f21D60)(uintptr_t a,uint32_t b,uint32_t c);
 uint16_t (*f5C1B0)();                            // FUN_6fb5c1b0; result = local_f (GUESS: decompiler lost the out value)
 void (*o10887)(uintptr_t a,uint32_t b,uint32_t c,uint32_t d);
 void (*f6A6C0)(); void (*f56210)();
};
void sub_AD4B0(Env& e,uint32_t a);
void sub_ADEF0(Env& e,uintptr_t p);
void sub_AEBE0(Env& e);
} // namespace d2client_batch_ad4b0
