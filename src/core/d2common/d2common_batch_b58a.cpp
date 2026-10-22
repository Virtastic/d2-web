// 1.13c D2Common implementation batch b58a: +0xD760, +0x30BD0.
// Skipped (register-argument / x87 / unrecovered-seam rows): 6ABE0, 7C0E0, 28D70, 86337, 87004,
// 10160, 102F0, 10AF0, 10CA0, 10EB0. Callees that are separate rows arrive through an env struct
// passed after the retail arguments; retail's fatal path (_exit(-1)) is modelled as a return.
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t R32(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
	inline uint16_t R16(const uint8_t* b, int o) { return *reinterpret_cast<const uint16_t*>(b + o); }
}

// Seams: 0x1330 (no args) -> room record; 0x66700 / 0x66800 / 0x665C0 (list, [room]) room-list upkeep.
struct D2CommonD760Env
{
	uint32_t (*get_room)(void);                       //0x00 0x1330
	void (*list_remove)(uint32_t pList);              //0x04 0x66700
	void (*list_add)(uint32_t pList, uint32_t pRoom); //0x08 0x66800
	void (*list_refresh)(uint32_t pList);             //0x0C 0x665C0
};

// 1.13c D2Common+0xD760: keep a path's room (+0x1C) valid. If the current room still contains the
// path's (x=+2, y=+6) word position (room box x +0x4C/+0x54, y +0x50/+0x58) nothing happens.
// Otherwise look the room up (retried when arg2 is set); when none is found and flag 0x40000 of
// +0x34 is set, clear +0x28 and stop. Else the old room moves to +0x20, the new one is installed
// (+0x34 |= 2) and the unit's room list (+0x30) is updated.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_d760(uint32_t pPath, int32_t nRetry, const D2CommonD760Env* pEnv)
{
	if (!pPath || !pEnv || !pEnv->get_room)
		return;
	uint8_t* path = P(pPath);
	const uint32_t pOld = R32(path, 0x1C);
	if (pOld) {
		const uint8_t* o = P(pOld);
		const int32_t x = R16(path, 2), y = R16(path, 6);
		if (static_cast<int32_t>(R32(o, 0x4C)) <= x && x < static_cast<int32_t>(R32(o, 0x54) + R32(o, 0x4C)) &&
		    static_cast<int32_t>(R32(o, 0x50)) <= y && y < static_cast<int32_t>(R32(o, 0x58) + R32(o, 0x50)))
			return;
	}
	uint32_t pNew = pEnv->get_room();
	if (!pNew && (!nRetry || !(pNew = pEnv->get_room())) && (R32(path, 0x34) & 0x40000)) {
		*reinterpret_cast<uint32_t*>(path + 0x28) = 0;
		return;
	}
	const uint32_t pList = R32(path, 0x30);
	*reinterpret_cast<uint32_t*>(path + 0x20) = R32(path, 0x1C);
	if (R32(path, 0x1C) && pEnv->list_remove)
		pEnv->list_remove(pList);
	*reinterpret_cast<uint32_t*>(path + 0x1C) = pNew;
	*reinterpret_cast<uint32_t*>(path + 0x34) |= 2;
	if (pNew) {
		if (pEnv->list_add) pEnv->list_add(pList, pNew);
		if (pEnv->list_refresh) pEnv->list_refresh(pList);
	}
}

// Seams for +0x30BD0: the three 0x14-stride .data tables (retail 0x6FDDC5F8 / 0x6FDDC788 / 0x6FDDC8C8),
// 0x303A0 (arg3) -> nonzero, 0x1250 () -> record whose byte +6 is masked, and gdwBitMasks[3].
struct D2Common30BD0Env
{
	const uint8_t* tbl0;               //0x00
	const uint8_t* tbl1_lo;            //0x04 used when nArg4 < 0x19A
	const uint8_t* tbl1_hi;            //0x08 used when nArg4 >= 0x19A
	int32_t (*check)(uint32_t nArg3);  //0x0C 0x303A0
	uint32_t (*get_record)(void);      //0x10 0x1250
	uint32_t nBitMask3;                //0x14 gdwBitMasks word at +0xC
};

// 1.13c D2Common+0x30BD0 (fastcall ECX,EDX + 2 stack): nMode 0 selects table 0, nMode 1 table 1
// (split on nArg4 at 0x19A); row stride 0x14. Row +0xC set -> 1; else row +8 set and the 0x303A0
// check passing -> byte +6 of the 0x1250 record masked with gdwBitMasks[3]; otherwise 0. Any other
// nMode is fatal in retail (_exit); modelled as 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_30bd0(int32_t nIndex, int32_t nMode, uint32_t nArg3, int32_t nArg4,
                                                   const D2Common30BD0Env* pEnv)
{
	if (!pEnv)
		return 0;
	const uint8_t* row;
	if (nMode == 0)
		row = pEnv->tbl0 + nIndex * 0x14;
	else if (nMode == 1)
		row = (nArg4 < 0x19A ? pEnv->tbl1_lo : pEnv->tbl1_hi) + nIndex * 0x14;
	else
		return 0;
	if (R32(row, 0xC))
		return 1;
	if (R32(row, 8) && pEnv->check && pEnv->check(nArg3) && pEnv->get_record)
		return static_cast<uint32_t>(P(pEnv->get_record())[6]) & pEnv->nBitMask3;
	return 0;
}
