// D2Common 0x4FB30: skill prerequisites met for a unit (stdcall, ret 8: pUnit, nSkill). Callees arrive through the Env seams.
#pragma once
#include <cstdint>
struct D2Common4fb30Env {
	uintptr_t tables;                                             // sgptDataTables (RVA 0x99E1C): the data-table block
	int32_t  (*requiredLevel)(uint32_t unit, int32_t skill);      // 0x4F510, stdcall(unit, skill)
	int32_t  (*statLevel)(uint32_t object, uint32_t rec);         // 0x38A80, stdcall(rec), object in EBX
	uint32_t (*rowOwned)(uint32_t object, uint32_t value);        // 0x4EB40, EAX = object, ECX = value
	int32_t  (*skillLevel)(uint32_t object, int32_t key);         // 0x4F4C0, EAX = object, EDX = key
	uint32_t (*skillRow)(int32_t id);                             // 0x1250: EAX = skill id, returns its Skills txt row (0 out of range)
};
extern "C" uint32_t D2Common_4FB30_SkillPrereqsMet(uint32_t pUnit, int32_t nSkill, const D2Common4fb30Env* env);
