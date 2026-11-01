// Shared callee env for the 1.13c D2Common maze builders 0xFD20-0x10BE0 (raw retail 32-bit layout; each port is its own file).
// Room (maze Room2): +0 first orth link {+0 room, +4 dir, +0x14 next}, +0x14/+0x18 LCG (low, high), +0x20 definition {+0 preset id,
// +4 sub id, +0xC flags, bit 1 = fixed}, +0x24 next room, +0x34/+0x38 x, y, +0x3C/+0x40 w, h, +0x58 level {+8 room count,
// +0x10 first room, +0x14 settings {+0x10/+0x14 default w, h}}.
// The callees keep their retail registers as explicit arguments.
#pragma once
#include <cstdint>

struct D2CommonMazeEnv
{
	uint32_t (*alloc)(uint32_t pLevelEBX, uint32_t nKind);                     // 0x44BD0: EBX = level, stack kind (2); a new room
	int32_t (*link)(uint32_t nDirEAX, uint32_t pRoomECX, uint32_t pNewEDX);    // 0xEEC0: 0 = the new room does not fit there
	void (*free_room)(uint32_t pRoom);                                         // 0x44F90: stack room
	void (*adjust)(uint32_t pRoomEAX, uint32_t pOtherESI, uint32_t nDir);      // 0x44950: add the orth link, stack dir
	void (*scatter)(uint32_t pRoom);                                           // 0xEB80: stack room
	void (*pick)(uint32_t pRoomEBX, uint32_t nReset);                          // 0xE490: EBX = room, stack 1
	uint32_t (*fog_alloc)(uint32_t pPoolECX, uint32_t nSizeEDX);               // 0x9222: Fog pool alloc (file/line/0 on the stack)
	void (*fog_free)(uint32_t pPoolECX, uint32_t pMemEDX);                     // 0x9228: Fog pool free
	uint32_t (*f000)(uint32_t pRoom, uint32_t nDir, uint32_t bScatter);        // 0xF000: stdcall
	void (*f10010)(uint32_t pLevelEAX, uint32_t nPreset, uint32_t pSkip);      // 0x10010: EAX = level, stack preset, skip room
	void (*f106b0)(const int32_t* pRecEDX, uint32_t pLevel, int32_t* pCounter); // 0x106B0: EDX = record, stack level, counter
	uint32_t (*f105c0)(uint32_t pLevelEAX, uint32_t nDir, uint32_t nA, uint32_t nB); // 0x105C0: EAX = level, stack dir, a, b
};

namespace d2common_maze
{
	inline uint32_t& D(uint32_t a) { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); }
	inline bool Fixed(uint32_t pRoom) { return ((D(D(pRoom + 0x20) + 0xC) >> 1) & 1) != 0; }
	// the shared head of every grow step: allocate, copy the default size from the level settings
	inline uint32_t NewRoom(const D2CommonMazeEnv* e, uint32_t pParent)
	{
		const uint32_t pNew = e->alloc(D(pParent + 0x58), 2);
		const uint32_t pSet = D(D(pNew + 0x58) + 0x14);
		D(pNew + 0x3C) = D(pSet + 0x10);
		D(pNew + 0x40) = D(pSet + 0x14);
		return pNew;
	}
	// put pNew at the head of pParent's level room list
	inline void Enlist(uint32_t pParent, uint32_t pNew)
	{
		const uint32_t pLevel = D(pParent + 0x58);
		D(pNew + 0x24) = D(pLevel + 0x10);
		const uint32_t n = D(pLevel + 8) + 1;
		D(pLevel + 0x10) = pNew;
		D(pLevel + 8) = n;
	}
	inline uint32_t Lcg(uint32_t pState)
	{
		const uint64_t v = static_cast<uint64_t>(D(pState)) * 0x6AC690C5ull + D(pState + 4);
		D(pState) = static_cast<uint32_t>(v);
		D(pState + 4) = static_cast<uint32_t>(v >> 32);
		return static_cast<uint32_t>(v);
	}
	// (n + 1) mod 4 with C's signed remainder, as retail writes it
	inline int32_t NextDir(int32_t n) { return (n + 1) % 4; }
	// the scan of 0x10730 / 0x10820 / 0x10A00: over the level's rooms, keep the one with the extreme coordinate that is not fixed,
	// has no link in direction nDir yet, and takes a new room there (the new room is linked, picked and freed again).
	inline uint32_t Probe(const D2CommonMazeEnv* e, uint32_t pLevel, int nKey, bool bMax, uint32_t nDir)
	{
		uint32_t pBest = 0;
		for (uint32_t r = D(pLevel + 0x10); r; r = D(r + 0x24))
		{
			if (pBest)
			{
				const int32_t a = static_cast<int32_t>(D(r + nKey)), b = static_cast<int32_t>(D(pBest + nKey));
				if (bMax ? a <= b : a >= b)
					continue;
			}
			if (Fixed(r))
				continue;
			bool bHas = false;
			for (uint32_t l = D(r); l; l = D(l + 0x14))
				if (D(l + 4) == nDir) { bHas = true; break; }
			if (bHas)
				continue;
			const uint32_t t = NewRoom(e, r);
			uint32_t pKeep = 0;
			if (!e->link(nDir, r, t))
				e->free_room(t);
			else
			{
				e->adjust(t, r, nDir);
				e->adjust(r, t, (nDir + 2) & 3);
				Enlist(r, t);
				e->pick(t, 1);
				pKeep = t;
			}
			if (pKeep)
			{
				e->free_room(pKeep);
				pBest = r;
			}
		}
		return pBest;
	}
}
