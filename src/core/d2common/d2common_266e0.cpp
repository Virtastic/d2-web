// 1.13c D2Common+0x266E0: sum a stat over a socketed item's contained items, 32-bit retail layout read directly
// (wasm64 unit/inventory structs are wider; 0x26080 does the same). stdcall(unit, statId, out); dt = sgptDataTables.
//   unit +0 type, +4 class id, +0x10 item mode, +0x14 item data, +0x5C stat list, +0x60 inventory;
//   inventory +0 signature 0x01020304, +0xC first item; item data +0x64 next item.
// Walk: out = 0; unit must be type 4, 0x22920 (socketable by class id) true and the inventory signature valid. Each node adds
// (when the host's mode is 1) the 0x38A80 total of statId<<16 on the node's stat list, 0 for a non-item node, a null list, an
// id outside [0, record count) or a null record; a non-item node ends the walk after its add, as do a null item data
// pointer or -0x5C.
#include <cstdint>
#include <cstring>

extern "C" int D2Common_10211_ITEMS_CheckIfSocketableByItemId(int nItemId);
extern "C" int32_t D2Common_38A80_StatTotal32(uint32_t list, int32_t key, uint32_t rec);

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_266E0_Items32(uint32_t unit, int32_t statId, int32_t* out, uint32_t dt)
{
	*out = 0;
	if (!unit || R32(unit) != 4 || !D2Common_10211_ITEMS_CheckIfSocketableByItemId(static_cast<int>(R32(unit + 4))))
		return;
	const uint32_t inv = R32(unit + 0x60);
	if (!inv || R32(inv) != 0x1020304u)
		return;
	for (uint32_t node = R32(inv + 0xC); node;) {
		const bool item = R32(node) == 4;
		if (R32(unit + 0x10) == 1) {
			int32_t add = 0;
			if (item && R32(node + 0x5C)) {
				const int32_t cnt = static_cast<int32_t>(R32(dt + 0xBD4));
				if (statId >= 0 && statId < cnt) {
					const uint32_t rec = static_cast<uint32_t>(statId) * 0x144u + R32(dt + 0xBCC);
					if (rec)
						add = D2Common_38A80_StatTotal32(R32(node + 0x5C), static_cast<int32_t>(static_cast<uint32_t>(statId) << 16), rec);
				}
			}
			*out += add;
		}
		if (!item)
			return;
		const uint32_t data = R32(node + 0x14);
		if (!data || data == 0xFFFFFFA4u)
			return;
		node = R32(data + 0x64);
	}
}
