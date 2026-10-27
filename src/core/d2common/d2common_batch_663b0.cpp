// 1.13c D2Common implementation batch 11: +0x663B0, +0x2EC00, +0x2EC60, +0x3C3B0, +0x1EC30,
// +0x24D00, +0x51070, +0x1EEE0, +0x3D2B0, +0x3C520, +0x4D6E0, +0x3D0F0.
// These are naming implementations: every branch written here is the 1.13c branch; helpers that
// are separate, still-UNMAPPED rows are named in ponytail notes and are not called.
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "D2Collision.h"
#include "inv_place.h"
#include "D2Dungeon.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/Units.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// 1.13c D2Common+0x663B0 (#10127): empty a room's update list.
// The list head is room+0x1C. Every linked unit stores its chain-next at +0xE0 and clears the
// UNITFLAG_ISLINKREFRESHMSG bit (0x2000) of its flags dword at +0xC4. Retail fatals 0x2AD on a
// null room; we return. List entries are 32-bit image addresses.
void D2Common_663B0_ClearRoomUpdateList(uint32_t pRoom)
{
	uint8_t* room = P(pRoom);
	if (!room)
		return;

	uint32_t unitAddress = W(room, 0x1C);
	while (unitAddress != 0)
	{
		uint8_t* unit = P(unitAddress);
		const uint32_t next = W(unit, 0xE0);
		W(unit, 0xE0) = 0;
		W(unit, 0xC4) &= 0xFFFFDFFFu;
		unitAddress = next;
	}
	W(room, 0x1C) = 0;
}

// 1.13c D2Common+0x3C3B0 (#10144): remove a unit id from the room's unit array.
// room+0x48 is an array of dword unit ids and room+0x78 its count. Retail first calls 0x3B3A0,
// which walks the array and fatals (0x453) on a null entry; that walk is inlined here. A found
// entry is overwritten with the last one and the count drops by one before the room's client
// list is refreshed. Retail fatals 0x487/0x488/0x490 on a null room, a zero id and a miss.
void D2Common_3C3B0_RemoveRoomUnit(uint32_t pRoom, uint32_t nUnitId)
{
	uint8_t* room = P(pRoom);
	if (!room || nUnitId == 0)
		return;

	const uint32_t count = W(room, 0x78);
	uint8_t* units = P(W(room, 0x48));

	for (uint32_t i = 0; i < count; ++i)
	{
		if (W(units, static_cast<int>(i * 4)) == 0)
			return;
	}

	uint32_t i = 0;
	while (i < count && W(units, static_cast<int>(i * 4)) != nUnitId)
		++i;

	if (i >= count)
		return;

	W(units, static_cast<int>(i * 4)) = W(units, static_cast<int>((count - 1) * 4));
	W(room, 0x78) = count - 1;
	DUNGEON_UpdateClientListOfRoom(reinterpret_cast<D2ActiveRoomStrc*>(room));
}

// 1.13c D2Common+0x1EC30 (#10145): count belt items of one item-data type. stdcall(inventory, nItemType).
// The inventory must carry magic 0x1020304 at +0x00. Grid 0 comes from 0x1E1B0 with the global
// 2-byte size record at 0x6FDED378 (pBeltInfo here); the walk starts at the grid's first item (+0x00)
// and follows item data (unit+0x14) -> +0x70. An item counts when unit type is 4, item data +0x00 is 5,
// item data +0x28 equals nItemType, and the dword at (unit+0x2C)+0xC is below 11. Retail fatals 0x1074
// on a null item data; the walk also stops on item data 0 or 0xFFFFFFA4.
int32_t D2Common_1EC30_CountBeltItems(const InvEnv& e, uint32_t pInventory, uint32_t nItemType, uint32_t pBeltInfo)
{
	uint8_t* inventory = P(pInventory);
	if (!inventory || W(inventory, 0) != 0x01020304u)
		return 0;
	uint8_t* grid = P(D2Common_1E1B0_GetGrid(e, pInventory, 0, pBeltInfo));
	if (!grid || !W(grid, 0))
		return 0;
	int32_t count = 0;
	uint8_t* unit = P(W(grid, 0));
	while (W(unit, 0) == 4)
	{
		uint8_t* data = P(W(unit, 0x14));
		if (data && W(data, 0) == 5 && W(data, 0x28) == nItemType && W(P(W(unit, 0x2C)), 0xC) < 0xB)
			++count;
		const uint32_t d = W(unit, 0x14);
		if (d == 0 || d == 0xFFFFFFA4u)
			break;
		const uint32_t next = W(P(d), 0x70);
		if (!next)
			break;
		unit = P(next);
	}
	return count;
}

// 1.13c D2Common+0x24D00 (#10146): base-variant check over items.txt.
// The item must have item type 0x32 or 0x2D (0x24500, the equivalence walk now marked in
// Items.cpp). Then, only when uber code (+0x88) or ultra code (+0x8C) repeats the base code
// (+0x80) and the item's primary type is not 0x26, the answer is whether nQuest (+0x12A) is
// zero. An out-of-range id answers false (retail fatals 0x1EF if the base pointer is null).
bool D2Common_24D00_ItemIsBaseVariantNotQuest(uint32_t nItemId)
{
	static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80, "items.txt dwCode");
	static_assert(offsetof(D2ItemsTxt, dwUberCode) == 0x88, "items.txt dwUberCode");
	static_assert(offsetof(D2ItemsTxt, dwUltraCode) == 0x8C, "items.txt dwUltraCode");
	static_assert(offsetof(D2ItemsTxt, nQuest) == 0x12A, "items.txt nQuest");

	const D2ItemsTxt* row = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nItemId));
	if (!row)
		return false;
	if (!ITEMS_CheckItemTypeIdByItemId(static_cast<int>(nItemId), 0x32) &&
	    !ITEMS_CheckItemTypeIdByItemId(static_cast<int>(nItemId), 0x2D))
		return false;
	if ((row->dwUberCode == row->dwCode || row->dwUltraCode == row->dwCode) &&
	    ITEMS_GetItemTypeFromItemId(nItemId) != 0x26)
		return row->nQuest == 0;
	return false;
}

// 1.13c D2Common+0x51070 (#10147): skills.txt field/stat dispatcher.
// nSkillId indexes skills.txt (row stride 0x23C); nSelector picks the field or formula below.
// The damage/mana values are the skills.txt fixed-point formulas: wLevelMana (+0x18C) per level
// above the first, plus wMana (+0x18A), shifted left by nManaShift (+0x188) & 0x1F, then >> 8
// for selectors 0x15/0x16. Selector 0x07 is the D2Common_11033 interpolation over dwParam[6]/[7].
// ponytail: the selectors not listed here dispatch to still-UNMAPPED rows 0x1670, 0x4E680,
// 0x16B0, 0x4E630, 0x16F0, 0x4E5E0, 0x1730, 0x1500, 0x1550, 0x1580, 0x15B0, 0x15E0, 0x1610,
// 0x1640, 0x4E900, 0x4E7A0, 0x38B70 and 0x4E480, so they answer 0. The raw-field cases are
// followed in retail by an indirection through the table at sgptDataTables+0x40 (Ordinal_10253);
// that table/ordinal is not modelled, so they return the raw dword.
uint32_t D2Common_51070_GetSkillStat(uint32_t pUnit, uint8_t nSelector, int32_t nSkillId, uint32_t nLevel)
{
	static_assert(offsetof(D2SkillsTxt, dwAuraLenCalc) == 0x60, "skills.txt dwAuraLenCalc");
	static_assert(offsetof(D2SkillsTxt, dwAuraRangeCalc) == 0x64, "skills.txt dwAuraRangeCalc");
	static_assert(offsetof(D2SkillsTxt, dwAuraStatCalc) == 0x68, "skills.txt dwAuraStatCalc");
	static_assert(offsetof(D2SkillsTxt, dwPassiveCalc) == 0xA4, "skills.txt dwPassiveCalc");
	static_assert(offsetof(D2SkillsTxt, dwPetMax) == 0xC0, "skills.txt dwPetMax");
	static_assert(offsetof(D2SkillsTxt, dwCalc) == 0x138, "skills.txt dwCalc");
	static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "skills.txt dwParam");
	static_assert(offsetof(D2SkillsTxt, dwSkPoints) == 0x170, "skills.txt dwSkPoints");
	static_assert(offsetof(D2SkillsTxt, nManaShift) == 0x188, "skills.txt nManaShift");
	static_assert(offsetof(D2SkillsTxt, wMana) == 0x18A, "skills.txt wMana");
	static_assert(offsetof(D2SkillsTxt, wLevelMana) == 0x18C, "skills.txt wLevelMana");
	static_assert(sizeof(D2SkillsTxt) == 0x23C, "skills.txt row size");

	const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(nSkillId);
	if (!row)
		return 0;

	const int32_t nLvl = static_cast<int32_t>(nLevel);
	D2UnitStrc* pSkillUnit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));
	const uint32_t nShift = row->nManaShift & 0x1Fu;

	switch (nSelector)
	{
	case 0x07:
		return static_cast<uint32_t>(D2Common_11033(nLvl, row->dwParam[6], row->dwParam[7]));

	case 0x0E: return static_cast<uint32_t>(row->dwParam[6]);
	case 0x0F: return static_cast<uint32_t>(row->dwParam[7]);
	case 0x10: return nLevel;

	case 0x11: return static_cast<uint32_t>(SKILLS_GetMinElemDamage(pSkillUnit, nSkillId, nLvl, 0)) >> 8;
	case 0x12: return static_cast<uint32_t>(SKILLS_GetMaxElemDamage(pSkillUnit, nSkillId, nLvl, 0)) >> 8;
	case 0x14: return static_cast<uint32_t>(SKILLS_GetToHitFactor(pSkillUnit, nSkillId, nLvl));

	case 0x15:
	case 0x16:
	case 0x2A:
	{
		if (nLvl < 1)
			return 0;
		int32_t nValue = static_cast<int32_t>(row->wLevelMana) * (nLvl - 1) + row->wMana;
		if (nSelector == 0x16)
			nValue = (nValue * 0x19) / 2;
		const uint32_t nShifted = static_cast<uint32_t>(nValue) << nShift;
		if (nSelector == 0x2A)
			return nShifted;
		return static_cast<uint32_t>(static_cast<int32_t>(nShifted) >> 8);
	}

	case 0x26: return static_cast<uint32_t>(SKILLS_GetMinElemDamage(pSkillUnit, nSkillId, nLvl, 0));
	case 0x27: return static_cast<uint32_t>(SKILLS_GetMaxElemDamage(pSkillUnit, nSkillId, nLvl, 0));
	case 0x31: return static_cast<uint32_t>(SKILLS_GetMinElemDamage(pSkillUnit, nSkillId, nLvl, 1)) >> 8;
	case 0x32: return static_cast<uint32_t>(SKILLS_GetMaxElemDamage(pSkillUnit, nSkillId, nLvl, 1)) >> 8;
	case 0x34: return static_cast<uint32_t>(SKILLS_GetMinElemDamage(pSkillUnit, nSkillId, nLvl, 1));
	case 0x35: return static_cast<uint32_t>(SKILLS_GetMaxElemDamage(pSkillUnit, nSkillId, nLvl, 1));

	case 0x36: return row->dwAuraLenCalc;
	case 0x37: return row->dwAuraRangeCalc;
	case 0x38: return row->dwCalc[0];
	case 0x39: return row->dwCalc[1];
	case 0x3A: return row->dwCalc[2];
	case 0x3B: return row->dwCalc[3];
	case 0x3C: return static_cast<uint32_t>(row->dwAuraStatCalc[0]);
	case 0x3D: return static_cast<uint32_t>(row->dwAuraStatCalc[1]);
	case 0x3E: return static_cast<uint32_t>(row->dwAuraStatCalc[2]);
	case 0x3F: return static_cast<uint32_t>(row->dwAuraStatCalc[3]);
	case 0x40: return static_cast<uint32_t>(row->dwAuraStatCalc[4]);
	case 0x41: return static_cast<uint32_t>(row->dwAuraStatCalc[5]);
	case 0x42: return row->dwPassiveCalc[0];
	case 0x43: return row->dwPassiveCalc[1];
	case 0x44: return row->dwPassiveCalc[2];
	case 0x45: return row->dwPassiveCalc[3];
	case 0x46: return row->dwPassiveCalc[4];
	case 0x47: return row->dwPetMax;
	case 0x48: return row->dwSkPoints;

	default: return 0;
	}
}

// 1.13c D2Common+0x1EEE0 (#10149, D2GetInventory): does the inventory belt hold a potion of
// this item's type? The inventory must carry magic 0x1020304 at +0x00; the item must be a
// UNIT_ITEM (type 4 at +0x00) whose items.txt code (+0x80) is neither 'isc ' (0x20637369) nor
// 'tsc ' (0x20637374). Retail then scans four cells at grid+0x0C and compares with 0x22CD0.
uint32_t D2Common_1EEE0_InventoryHasPotion(uint32_t pInventory, uint32_t pItem)
{
	static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80, "items.txt dwCode");

	uint8_t* inventory = P(pInventory);
	if (!inventory || W(inventory, 0) != 0x01020304u)
		return 0;

	uint8_t* item = P(pItem);
	if (!item || W(item, 0) != 4)
		return 0;

	const D2ItemsTxt* row = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(W(item, 4)));
	if (!row || row->dwCode == 0x20637369u || row->dwCode == 0x20637374u)
		return 0;

	// ponytail: the four-cell scan needs the grid from 0x1E1B0 with retail's environment
	// (DAT_6fded390) and ITEMS_ComparePotionTypes (0x22CD0, implemented). Until the environment
	// is wired here the answer is "no match".
	return 0;
}

// 1.13c D2Common+0x3D2B0 (#10151): sort a text list's entries.
// The list head is the dword at +8 and the entry count is the u16 at +4; a null list fatals
// 0x165. With a count of 0 or 1 retail does nothing. Otherwise it copies every node (u16 id at
// +0, dword value at +4, next at +8) into a scratch array, sorts it with 0x3CF30 and the
// comparator at LAB_6fd8d040, copies the entries back into the nodes in list order, and frees
// the scratch array.
// Comparator at 0x3D040: 1 when the first u16 id is greater, -1 when smaller, 0 when equal.
extern "C" void d2_fn_d2common_3cf30(uint32_t records, uint32_t count, int32_t (*compare)(uintptr_t, uintptr_t));
static int32_t TextCompare_3D040(uintptr_t a, uintptr_t b)
{
	const uint16_t x = *reinterpret_cast<const uint16_t*>(a), y = *reinterpret_cast<const uint16_t*>(b);
	return x > y ? 1 : (x < y ? -1 : 0);
}

void D2Common_3D2B0_SortTextList(uint32_t pList)
{
	uint8_t* list = P(pList);
	if (!list)
		return;
	const uint16_t count = *reinterpret_cast<uint16_t*>(list + 4);
	if (count < 2)
		return;
	// ponytail: retail allocs via ordinal 10045 (unmapped); the scratch is unobservable, so calloc.
	uint8_t* scratch = static_cast<uint8_t*>(calloc(1, 12u * count));
	if (!scratch)
		return;
	uint8_t* out = scratch;
	for (uint32_t n = W(list, 8); n; n = W(P(n), 8), out += 12)
	{
		*reinterpret_cast<uint16_t*>(out) = *reinterpret_cast<uint16_t*>(P(n));
		W(out, 4) = W(P(n), 4);
	}
	d2_fn_d2common_3cf30(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(scratch)), count, TextCompare_3D040);
	out = scratch;
	uint32_t n = W(list, 8);
	for (uint32_t i = 0; i < count; ++i, out += 12)
	{
		*reinterpret_cast<uint16_t*>(P(n)) = *reinterpret_cast<uint16_t*>(out);
		W(P(n), 4) = W(out, 4);
		n = W(P(n), 8);
	}
	free(scratch);
}

// 1.13c D2Common+0x3C520 (#10163): give the room the text list it currently lacks.
// Retail fatals 0x3DD on a null room, 0x3DE when room+0x50 is already set, and 0x3DF on a null
// text list; otherwise it calls 0x7BA00(0, *(int*)(pText + 0x10)).
// The 0x7BA00 call is a pure predicate whose result is discarded, so the only retail effects are the
// three fatal exits (_exit(-1)); this port returns on those, matching the file's convention.
void D2Common_3C520_AssignTextToRoom(uint32_t pRoom, uint32_t pText)
{
	uint8_t* room = P(pRoom);
	if (!room)
		return;
	if (W(room, 0x50) != 0)
		return;

	uint8_t* text = P(pText);
	if (!text)
		return;
}

// 1.13c D2Common+0x4D6E0 (#10166): move a collision footprint one cell.
// The footprint mask is cleared at the source and the move-condition mask is tested at the
// destination with the same collision pattern. On a hit the footprint is restored at the source
// and the hit mask returned; on a miss it is placed at the destination and 0 returned.
// The vendored COLLISION_TryTeleportUnitCollisionMask (+0x44950) is the same rule; this row is
// the caller-facing copy and calls the three primitives directly.
uint32_t D2Common_4D6E0_MoveCollisionMask(uint32_t pRoom, int32_t nX1, int32_t nY1, int32_t nX2, int32_t nY2,
                                          int32_t nCollisionPattern, uint16_t nFootprintMask, uint16_t nMoveConditionMask)
{
	D2ActiveRoomStrc* room = reinterpret_cast<D2ActiveRoomStrc*>(static_cast<uintptr_t>(pRoom));

	COLLISION_ResetMaskWithPattern(room, nX1, nY1, nCollisionPattern, nFootprintMask);
	const uint16_t nCollidedWithMask = COLLISION_CheckMaskWithPattern(room, nX2, nY2, nCollisionPattern, nMoveConditionMask);

	if (nCollidedWithMask == 0)
	{
		COLLISION_SetMaskWithPattern(room, nX2, nY2, nCollisionPattern, nFootprintMask);
		return 0;
	}

	COLLISION_SetMaskWithPattern(room, nX1, nY1, nCollisionPattern, nFootprintMask);
	return nCollidedWithMask;
}

// 1.13c D2Common+0x3D0F0 (#10167): id of the n-th counting text-list entry.
// Nodes hang off the dword at list+8. A node whose dword at +4 is 1 is a counted entry; the
// walk returns its u16 id at +0 when the running index matches, else continues at the node's
// next dword at +8. Running off the chain answers 0xFFFF.
uint32_t D2Common_3D0F0_GetTextListIdByIndex(uint32_t pList, int32_t nIndex)
{
	uint8_t* list = P(pList);
	if (!list)
		return 0xFFFF;

	uint32_t nodeAddress = W(list, 8);
	int32_t nSeen = 0;

	for (;;)
	{
		if (nodeAddress == 0)
			return 0xFFFF;

		uint8_t* node = P(nodeAddress);
		if (W(node, 4) == 1)
		{
			if (nSeen == nIndex)
				return *reinterpret_cast<uint16_t*>(node);
			++nSeen;
		}
		nodeAddress = W(node, 8);
	}
}

// 1.13c D2Common+0x3C460 (#10079): append a client pointer to a room's client array, in the
// retail room layout (array +0x48, count +0x78, capacity +0x04). The vendored
// DUNGEON_AddClientToRoom works on the 1.10f engine layout; this is the retail-layout form the
// FN capture replays. Retail fatals (0x473/0x474/0x475/0x477) become silent returns. When
// count == capacity the capacity grows by 4 (realloc, pool-less: ponytail, capture never grows).
// Then 0x3BDD0 re-sorts the array ascending by pointer value (bubble passes, early-out).
uint32_t D2Common_3C460_AddClientToRoom(uint32_t pRoom, uint32_t pClient)
{
	uint8_t* room = P(pRoom);
	if (!room || !pClient)
		return 0;
	uint32_t n = W(room, 0x78);
	uint32_t* a = reinterpret_cast<uint32_t*>(P(W(room, 0x48)));
	for (uint32_t i = 0; i < n; ++i)
		if (!a[i])
			return 0;
	if (n > W(room, 0x04))
		return 0;
	if (n == W(room, 0x04))
	{
		W(room, 0x04) = n + 4;
		a = static_cast<uint32_t*>(std::realloc(a, (n + 4) * 4));
		W(room, 0x48) = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(a));
	}
	a[n++] = pClient;
	W(room, 0x78) = n;
	// eax at return, as retail leaves it (the capture records it): &a[i] of the last compare,
	// the array base after a swap, or the count when the passes run out (0x3BDD0's exits).
	uint32_t eax = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(a));
	for (int32_t k = static_cast<int32_t>(n) - 1; k > 0; --k)
	{
		bool swapped = false;
		for (int32_t i = 0; i < k; ++i)
		{
			eax = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(a + i));
			if (a[i] > a[i + 1])
			{
				uint32_t t = a[i]; a[i] = a[i + 1]; a[i + 1] = t;
				swapped = true;
				eax = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(a));
			}
		}
		if (!swapped)
			return eax;
	}
	return n;
}
