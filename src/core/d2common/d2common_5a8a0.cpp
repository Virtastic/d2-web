// 1.13c D2Common+0x5A8A0 (stdcall nCount, ret 4): monstats walk/run animation speeds.
// Rows (stride 0x1A8) come from the module pointer at +0xA0950. Per row i: a base id (+0x02, signed) outside [0, nCount) is
// reset to i. Walk = the animdata speed (record +0x0C) of (base, mode 2), found by building the key with 0x43C30 and looking it
// up with 0x41DD0 (EBX = key buffer; NULL falls back to [+0xA0B4C] + 0x404). When base != i and the base's velocity (+0x32) is
// positive, walk = (velocity * walk) / base velocity as an unsigned 32-bit divide. Clamped to 0..0x7FFF, stored at +0x36.
// Run: rows below 410 take the base row's +0x36 halved (signed), the rest the animdata speed of (base, mode 0xF); scaled by
// +0x34 the same way, same clamp, stored at +0x38.
#include <cstdint>
#include <cstring>

#include "D2DataTbls.h"
#include "DataTbls/AnimTbls.h"
#include "Units/Units.h"

struct D2Common5A8A0Env {
	uint8_t* rows;                                                     // [+0xA0950]
	void (*key)(int32_t nBase, int32_t nMode, uint32_t* pBuf, uint32_t* pName);   // 0x43C30(0, base, mode, 1, 0, pBuf, pName, 0, 1)
	const uint8_t* (*lookup)(uint32_t* pBuf);                          // 0x41DD0, EBX = pBuf
	const uint8_t* fallback;                                           // [+0xA0B4C] + 0x404
};

namespace {
inline int32_t S16(const uint8_t* p, int32_t o) { int16_t v; std::memcpy(&v, p + o, 2); return v; }
inline void W16(uint8_t* p, int32_t o, int32_t v) { const int16_t x = static_cast<int16_t>(v); std::memcpy(p + o, &x, 2); }
inline int32_t Speed(const D2Common5A8A0Env* e, int32_t nBase, int32_t nMode) {
	uint32_t buf[3] = { 0, 0, 0 }, name = 0;
	e->key(nBase, nMode, buf, &name);
	const uint8_t* rec = e->lookup(buf);
	if (!rec) rec = e->fallback;
	int32_t v; std::memcpy(&v, rec + 0x0C, 4); return v;
}
inline int32_t Scale(int32_t v, const uint8_t* row, const uint8_t* base, int32_t o, bool other) {
	if (other && S16(base, o) > 0)
		v = static_cast<int32_t>(static_cast<uint32_t>(S16(row, o)) * static_cast<uint32_t>(v) / static_cast<uint32_t>(S16(base, o)));
	return v < 1 ? 0 : (v < 0x7FFF ? v : 0x7FFF);
}
}

// Returns nCount: retail reloads the count argument into EAX for the loop test and leaves it there.
extern "C" int32_t D2Common_5A8A0_MonAnimSpeeds(int32_t nCount, const D2Common5A8A0Env* e) {   // 1.13c D2Common+0x5A8A0
	for (int32_t i = 0; i < nCount; ++i) {
		uint8_t* row = e->rows + i * 0x1A8;
		int32_t b = S16(row, 0x02);
		if (b < 0 || b >= nCount) { W16(row, 0x02, i); b = i; }
		W16(row, 0x36, Scale(Speed(e, b, 2), row, e->rows + b * 0x1A8, 0x32, b != i));
		const int32_t r = i < 410 ? S16(e->rows + b * 0x1A8, 0x36) / 2 : Speed(e, b, 0x0F);
		W16(row, 0x38, Scale(r, row, e->rows + b * 0x1A8, 0x34, b != i));
	}
	return nCount;
}

// Play path: the 1.10f animdata lookup builds and finds the same key (unit type monster, no unit, no inventory).
namespace {
int32_t s_mode, s_base;
void PlayKey(int32_t nBase, int32_t nMode, uint32_t*, uint32_t*) { s_base = nBase; s_mode = nMode; }
const uint8_t* PlayLookup(uint32_t*) {
	return reinterpret_cast<const uint8_t*>(DATATBLS_GetAnimDataRecord(nullptr, s_base, s_mode, UNIT_MONSTER, nullptr));
}
}
extern "C" void D2Common_5A8A0_MonAnimSpeedsPlay(int32_t nCount) {
	static const uint8_t kZero[0x10] = {};
	const D2Common5A8A0Env e = { reinterpret_cast<uint8_t*>(sgptDataTables->pMonStatsTxt), PlayKey, PlayLookup, kZero };
	D2Common_5A8A0_MonAnimSpeeds(nCount, &e);
}
