// D2Common 0x47120 port (d2common_batch_47120.cpp): retail callees and sgptDataTables as seams.
#pragma once
#include <cstdint>
struct D2Common47120Env {
 uint32_t (*dt)();                                                   // sgptDataTables
 uint32_t (*find)(uint32_t par, uint32_t flt);                       // 0x37F40
 int32_t (*value)(uint32_t list, uint32_t key, uint32_t rec);        // 0x38B00
 void (*set)(uint32_t list, uint32_t stat, uint32_t v, uint32_t layer, uint32_t unit);   // 0x3A1D0
};
int32_t D2Common_47120_AddChargedSkill(uint32_t unit, int32_t skill, uint32_t level, int32_t charges, const D2Common47120Env* e);
