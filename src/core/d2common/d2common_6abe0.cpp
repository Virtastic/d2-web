// 1.13c D2Common+0x6ABE0 (sub_6ABE0): outlined middle of the missile damage-data builder. Retail
// takes the unit on the stack, the shift in a register and the damage struct in ESI; here all three
// are explicit. Adds unit stats into the damage struct: stat ids 0x30..0x37 (fire/light/magic/cold
// min+max) shifted left by nShift into +0x0C,0x10,0x18..0x2C; 0x38..0x3A (cold length, poison
// min/max) into +0x30..0x38 unshifted; then +0x3C gets stat 0x65 when it is positive (and +0x40 is
// skipped), else stat 0x3C into +0x3C and stat 0x147 into +0x40; then 0x3D->+0x44, 0x3F->+0x4C,
// 0x13D->+0x60, 0x13E->+0x64, 0x13C->+0x68. A stat reads as 0 unless the unit has a stat list
// (+0x5C), the stat id is below the table count (sgptDataTables+0xBD4) and its row
// (table +0xBCC, stride 0x144) exists. Each stat read is the inlined body of 0x38B70: 0x38A80 called with
// the unit's stat list (+0x5C) in EBX, the packed id (stat id << 16, layer 0) in EDI and the row on the stack.
#include <cstdint>
#include <emscripten.h>

struct D2Common6ABE0Env
{
	uint32_t pDataTables;                        // *sgptDataTables
	int32_t (*get_total_stat)(uint32_t pStatList, uint32_t nPackedId, uint32_t pStatRow); // 0x38A80 (EBX, EDI, stack)
};

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6abe0(uint32_t pUnit, uint32_t nShift, uint32_t pDmg, const D2Common6ABE0Env* env)
{
	if (!pDmg || !env)
		return;
	auto rd = [](uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); };
	const uint32_t t = env->pDataTables;
	const bool has = pUnit && rd(pUnit + 0x5C);
	auto stat = [&](uint32_t id) -> int32_t {
		if (!has || static_cast<int32_t>(rd(t + 0xBD4)) <= static_cast<int32_t>(id) || !(rd(t + 0xBCC) + id * 0x144))
			return 0;
		return env->get_total_stat(rd(pUnit + 0x5C), id << 16, rd(t + 0xBCC) + id * 0x144);
	};
	auto add = [&](uint32_t off, int32_t v) {
		*reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pDmg + off)) += static_cast<uint32_t>(v);
	};
	const uint32_t s = nShift & 0x1F;
	static const uint8_t shifted[8][2] = {{0x30, 0x0C}, {0x31, 0x10}, {0x32, 0x18}, {0x33, 0x1C},
	                                      {0x34, 0x20}, {0x35, 0x24}, {0x36, 0x28}, {0x37, 0x2C}};
	for (auto& e : shifted)
		add(e[1], static_cast<int32_t>(static_cast<uint32_t>(stat(e[0])) << s));
	add(0x30, stat(0x38));
	add(0x34, stat(0x39));
	add(0x38, stat(0x3A));
	int32_t v = stat(0x65);
	if (v > 0) {
		add(0x3C, v);
	} else {
		add(0x3C, stat(0x3C));
		add(0x40, stat(0x147));
	}
	add(0x44, stat(0x3D));
	add(0x4C, stat(0x3F));
	add(0x60, stat(0x13D));
	add(0x64, stat(0x13E));
	add(0x68, stat(0x13C));
}
