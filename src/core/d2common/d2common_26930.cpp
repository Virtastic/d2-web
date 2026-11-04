// D2Common 0x26930 port, written from the 1.13c decompile and the retail instruction order.
#include <cstdint>
#include "d2common_26930.h"

static inline uint32_t rd32(uintptr_t a) { return *reinterpret_cast<const uint32_t*>(a); }

// Class at +4 (or -1 for null), type at +0 must be 4 with item data at +0x14, else quality 2.
static uint32_t classOf(uint32_t p) { return p ? rd32(p + 4) : 0xFFFFFFFFu; }
static uint32_t typeQuality(uint32_t p) {
	if (!p || rd32(p) != 4 || rd32(p + 0x14) == 0) return 2;
	return rd32(rd32(p + 0x14));
}

// 1.13c D2Common+0x26930
extern "C" uint32_t D2Common_26930_AreStackablesEqual(uint32_t pFirst, uint32_t pSecond, const D2Common26930Env* env)
{
	if (classOf(pFirst) != classOf(pSecond)) return 0;
	if (typeQuality(pFirst) != typeQuality(pSecond)) return 0;
	if (env->fileIndex(pFirst) != env->fileIndex(pSecond)) return 0;
	if (!(env->stackable(pFirst) & 0xFF)) return 0;
	if (env->ethereal(pFirst) != env->ethereal(pSecond)) return 0;
	// 0x21F70 inline: quality is valid when nonzero and outside 4..9.
	const int32_t q1 = env->quality(pFirst);
	if (!(q1 != 0 && (q1 < 4 || q1 > 9))) return 0;
	const int32_t q2 = env->quality(pSecond);
	if (!(q2 != 0 && (q2 < 4 || q2 > 9))) return 0;
	static const int32_t kStats[] = { 0x15, 0x16, 0x17, 0x18, 0x9F, 0xA0 };
	for (int32_t id : kStats)
		if (env->statValue(pFirst, id) != env->statValue(pSecond, id)) return 0;
	if (static_cast<int8_t>(env->opStatTotal(pFirst)) != 0) return 0;
	if (static_cast<int8_t>(env->opStatTotal(pSecond)) != 0) return 0;
	return 1;
}
