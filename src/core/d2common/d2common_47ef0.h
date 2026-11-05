// D2Common 0x47EF0 / 0x47FA0 ports (d2common_batch_47ef0.cpp, d2common_batch_47fa0.cpp): an item property adds (or revokes) a
// stat on a child stat list. Retail callees as seams, retail registers named on each.
#pragma once
#include <cstdint>
struct D2Common47EF0Env {
 void (*apply)(uint32_t unit, uint32_t stat);                                 // 0x46E50: ESI unit, one stack word (stat)
 void (*shift)(uint32_t stat, int32_t* value);                                // 0x45640: EAX stat, ECX &value (scales it in place)
 uint32_t (*child)(uint32_t self, uint32_t owner, uint32_t id, uint32_t flt);  // 0x47020: ECX self, EAX owner, stack id, flt
 int32_t (*get)(uint32_t list, uint32_t stat, uint32_t layer);                // 0x39140 stdcall, ret 0xC
 void (*add)(uint32_t list, uint32_t stat, int32_t v, uint32_t layer);         // 0x3A330 stdcall, ret 0x10
 int32_t (*resolve463f0)(uint32_t ecx, uint32_t eax, uint32_t edx);           // 0x47FA0 only: EAX [p+0xC], ECX [p+8], EDX item
 int32_t (*resolve464d0)(uint32_t ecx, uint32_t eax, uint32_t edx);
 void (*fatal)(uint32_t code);                                                // Fog 10265(code) / 10024, then _exit(-1)
};
uint32_t D2Common_47EF0_ApplyStat(uint32_t gate, uint32_t unit, uint32_t stat, int32_t mode, uint32_t self, int32_t value,
                                  int32_t revoke, uint32_t id, uint32_t flt, const D2Common47EF0Env* e);
uint32_t D2Common_47FA0_ApplyResolvedStat(uint32_t gate, uint32_t prop, uint32_t unit, uint32_t stat, int32_t mode, uint32_t self,
                                          int32_t revoke, uint32_t id, uint32_t flt, const D2Common47EF0Env* e);
