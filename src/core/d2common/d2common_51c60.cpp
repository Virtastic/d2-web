// 1.13c D2Common+0x51C60: unit base damage, stdcall(unit, &min, &max, &bonus), void. Retail-layout (32-bit) reader; `dt` is the
// data-table block the module pointer at RVA 0x99E1C holds. The twelve callees are seams:
//   totalStat 0x38A80 (EBX = the unit's stat list, EDI = the packed key, stack: the itemstatcost row), leftWeapon 0x20320 (inventory),
//   itemAtLoc 0x1E670 (inventory, body location), isType 0x24430 (item, type), equippable 0x23EF0 (item), merge 0x3AC90 (unit, item, flag),
//   invRecord 0x1E1B0 (EBX = inventory, EAX = index 0), itemTxt 0x719A0 (class id), strBonus 0x229A0 / dexBonus 0x22970 (item, a signed
//   16-bit result), unitStat 0x38B70 (unit, stat, layer), ratio 0x11E0 (EAX = the bonus, ECX = 100, EDX = the stat).
// Unit: +0x5C stat list, +0x60 inventory. Item: +0 type, +0x14 data (+0x44 class byte), +0x5C list -> +0x34 parent (+0x10 flags, +0x44 owner).
#include <cstdint>
#include <cstring>

struct D2Common51C60Env {
	int32_t (*totalStat)(uint32_t, uint32_t, uint32_t); uint32_t (*leftWeapon)(uint32_t); uint32_t (*itemAtLoc)(uint32_t, uint32_t);
	uint32_t (*isType)(uint32_t, uint32_t); uint32_t (*equippable)(uint32_t); void (*merge)(uint32_t, uint32_t, uint32_t);
	uint32_t (*invRecord)(uint32_t, uint32_t); uint32_t (*itemTxt)(uint32_t); int32_t (*strBonus)(uint32_t); int32_t (*dexBonus)(uint32_t);
	int32_t (*unitStat)(uint32_t, uint32_t, uint32_t); int32_t (*ratio)(int32_t, int32_t, int32_t); };

namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline uint8_t Rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
inline void Add32(uint32_t a, uint32_t d) { uint32_t v = Rd32(a) + d; memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
// The item's own list has a parent that is flagged (sign bit) and owned.
inline bool OwnedList(uint32_t item)
{
	const uint32_t list = Rd32(item + 0x5C);
	if (list == 0) return false;
	const uint32_t parent = Rd32(list + 0x34);
	if (parent == 0) return false;
	return static_cast<int32_t>(Rd32(parent + 0x10)) < 0 && Rd32(parent + 0x44) != 0;
}
}

extern "C" void D2Common_51C60_Raw(uint32_t dt, uint32_t unit, uint32_t pMin, uint32_t pMax, uint32_t pBonus, const D2Common51C60Env* env)
{
	if (unit == 0) return;

	uint32_t base = 0;
	const uint32_t list = Rd32(unit + 0x5C);
	if (list != 0 && static_cast<int32_t>(Rd32(dt + 0xBD4)) > 0x89) {
		const uint32_t row = Rd32(dt + 0xBCC) + 0xAD64;
		if (row != 0) base = static_cast<uint32_t>(env->totalStat(list, 0x890000, row));
	}
	Add32(pMin, base);
	Add32(pMax, base);

	const uint32_t inv = Rd32(unit + 0x60);
	if (inv == 0) return;

	uint32_t weapon = env->leftWeapon(inv);
	uint32_t other = 0;
	if (weapon != 0) {
		uint8_t cls = 0;
		if (Rd32(weapon) == 4) {
			const uint32_t data = Rd32(weapon + 0x14);
			if (data != 0) cls = Rd8(data + 0x44);
		}
		other = env->itemAtLoc(inv, cls == 4 ? 5u : 4u);
	}

	// the type test runs on a null weapon too
	if (env->isType(weapon, 0x2D) != 0 && env->equippable(weapon) != 0 && weapon != 0 && OwnedList(weapon)) env->merge(unit, weapon, 0);
	else weapon = 0;

	if (other != 0 && other != weapon && env->isType(other, 0x2D) != 0 && env->equippable(other) != 0 && OwnedList(other))
		env->merge(unit, other, 0);

	if (Rd32(inv) == 0x01020304) {
		const uint32_t rec = env->invRecord(inv, 0);
		if (rec != 0) {
			const uint32_t item = Rd32(Rd32(rec + 0xC) + 0x24);
			if (item != 0 && Rd32(item) == 4) {
				const uint32_t txt = env->itemTxt(Rd32(item + 4));
				if (txt != 0) {
					const int32_t lo = static_cast<int32_t>(Rd8(txt + 0xFE) + base);
					int32_t hi = static_cast<int32_t>(Rd8(txt + 0xFF) + base);
					uint32_t pct = 0;
					const int32_t str = env->strBonus(item);
					if (str != 0) pct = static_cast<uint32_t>(env->ratio(str, 100, env->unitStat(unit, 0, 0)));
					const int32_t dex = env->dexBonus(item);
					if (dex != 0) pct += static_cast<uint32_t>(env->ratio(dex, 100, env->unitStat(unit, 2, 0)));
					pct += static_cast<uint32_t>(env->unitStat(unit, 0x19, 0));
					if (static_cast<int32_t>(pct) <= -0x5A) pct = static_cast<uint32_t>(-0x5A);
					const uint32_t flat = static_cast<uint32_t>(env->unitStat(unit, 0x11, 0));
					if (lo >= hi) hi = lo;
					Add32(pBonus, flat + pct);
					Add32(pMin, static_cast<uint32_t>(lo));
					Add32(pMax, static_cast<uint32_t>(hi));
				}
			}
		}
	}

	if (weapon != 0) env->merge(unit, weapon, 1);
}
