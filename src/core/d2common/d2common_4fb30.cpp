// D2Common 0x4FB30 port, written from the 1.13c decompile and the retail instruction order.
#include <cstdint>
#include "d2common_4fb30.h"

static inline uint32_t rdu(uintptr_t a) { return *reinterpret_cast<const uint32_t*>(a); }
static inline int32_t rdi(uintptr_t a) { return *reinterpret_cast<const int32_t*>(a); }
static inline int32_t rds(uintptr_t a) { return *reinterpret_cast<const int16_t*>(a); }

// 1.13c D2Common+0x4FB30
extern "C" uint32_t D2Common_4FB30_SkillPrereqsMet(uint32_t pUnit, int32_t nSkill, const D2Common4fb30Env* env)
{
	const uintptr_t T = env->tables;
	const int32_t count = rdi(T + 0xBA0);
	if (nSkill < 0 || nSkill >= count) return 0;
	const uintptr_t base = rdu(T + 0xB98);
	const uintptr_t row = base + (uintptr_t)(uint32_t)(nSkill * 0x23C);
	if (row == 0) return 0;

	// Unit level: 0 unless the unit has a sub-object, the table version is >= 0xD and the stat record is set.
	int32_t level = 0;
	if (pUnit != 0 && rdu(pUnit + 0x5C) != 0) {
		const uint32_t rec = rdu(T + 0xBCC) + 0xF30;
		if (!(rdi(T + 0xBD4) < 0xD) && rec != 0)
			level = env->statLevel(rdu(pUnit + 0x5C), rec);
	}

	if (env->requiredLevel(pUnit, nSkill) > level) return 0;

	const int32_t p1 = rds(row + 0x17E);
	if (p1 < 0) {
		if (env->requiredLevel(pUnit, nSkill) > level) return 0;
	} else {
		if (p1 >= count) return 0;
		const uintptr_t row1 = base + (uintptr_t)(uint32_t)(p1 * 0x23C);
		if (row1 == 0) return 0;
		if (!env->rowOwned(pUnit, (uint32_t)row1)) return 0;
		if (env->skillLevel(pUnit, p1) < 1) return 0;
	}

	const int32_t p2 = rds(row + 0x180);
	if (p2 >= 0) {
		const uint32_t pl = env->player();
		if (pl == 0) return 0;
		if (!env->rowOwned(pUnit, pl)) return 0;
		if (env->skillLevel(pUnit, p2) < 1) return 0;
	}

	const int32_t p3 = rds(row + 0x182);
	if (p3 >= 0) {
		const uint32_t pl = env->player();
		if (pl == 0) return 0;
		if (!env->rowOwned(pUnit, pl)) return 0;
		if (env->skillLevel(pUnit, p3) < 1) return 0;
	}
	return 1;
}
