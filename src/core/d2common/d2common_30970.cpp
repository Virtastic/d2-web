// 1.13c D2Common+0x30970 (D2InitPlayerData: player record setup).
// stdcall 1: the argument is a unit-like owner whose +0x08 is the memory pool and +0x14 the record slot.
//   null owner: fatal 0xF4F; slot already set: fatal 0xF51 (both _exit(-1) in retail).
//   record = pool alloc 0x16C (Units.cpp:0xF52, no zero flag), zeroed, record[0] = 0, stored at owner+0x14.
//   for off = 0x1C, 0x20, 0x24: a 0x14-byte quest block (QuestRecord.cpp:0x50) and a 0x60-byte scratch
//   (QuestRecord.cpp:0x51, zeroed) are allocated; the scratch is copied over the block for 0x60 bytes
//   (Fog/Storm #10126; the block is only 0x14 long, retail's overrun goes into the pool) and the block is
//   stored at record[off-0xC]; then a 0x10-byte waypoint block (Waypoint.cpp:0xFC) is zeroed, word 0 := 0x0102,
//   the u16 at word index [D2Common+0x99C58] gets [D2Common+0x99C5A] OR-ed in, and it is stored at record[off].
//   record[0x2C] = 1 << index of the first entry == 1 in the int array [D2Common+0x9F1420] (count at +0x9F1424),
//   0 when none; record[0x4C] = 0.
// The scratch block is never freed (retail leaks it).
#include <cstdint>
#include <cstring>

struct D2Common30970Env {
	uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t fileRva, uint32_t line, uint32_t zeroFlag);
	void (*copy)(uint32_t dst, uint32_t src, uint32_t n);
	void (*fatal)(uint32_t line);
	uint32_t wpIndex;      // u16 at D2Common+0x99C58
	uint32_t wpBits;       // u16 at D2Common+0x99C5A
	uint32_t flagsPtr;     // D2Common+0x9F1420
	int32_t flagsCount;    // D2Common+0x9F1424
};

static uint32_t cpRd(uint32_t a)
{
	uint32_t v;
	std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4);
	return v;
}
static void cpWr(uint32_t a, uint32_t v)
{
	std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4);
}
static void cpZero(uint32_t a, uint32_t n)
{
	std::memset(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), 0, n);
}

// 1.13c D2Common+0x30970
extern "C" void d2common_30970_run(uint32_t owner, const D2Common30970Env* env)
{
	if (owner == 0)
	{
		env->fatal(0xF4F);
		return;
	}
	if (cpRd(owner + 0x14) != 0)
	{
		env->fatal(0xF51);
		return;
	}
	const uint32_t rec = env->alloc(cpRd(owner + 8), 0x16C, 0x94644, 0xF52, 0);
	cpWr(owner + 0x14, rec);
	cpZero(rec, 0x5B * 4);
	*reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(rec)) = 0;
	for (uint32_t off = 0x1C; off < 0x28; off += 4)
	{
		const uint32_t quest = env->alloc(cpRd(owner + 8), 0x14, 0x94704, 0x50, 0);
		const uint32_t scratch = env->alloc(cpRd(owner + 8), 0x60, 0x94704, 0x51, 0);
		cpZero(scratch, 0x18 * 4);
		env->copy(quest, scratch, 0x60);
		cpWr(cpRd(owner + 0x14) + off - 0xC, quest);
		const uint32_t wp = env->alloc(cpRd(owner + 8), 0x10, 0x90118, 0xFC, 0);
		cpZero(wp, 16);
		const uint16_t type = 0x0102;
		std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(wp)), &type, 2);
		uint16_t w;
		const uintptr_t bit = static_cast<uintptr_t>(wp) + (env->wpIndex & 0xFFFF) * 2;
		std::memcpy(&w, reinterpret_cast<const void*>(bit), 2);
		w = static_cast<uint16_t>(w | (env->wpBits & 0xFFFF));
		std::memcpy(reinterpret_cast<void*>(bit), &w, 2);
		cpWr(cpRd(owner + 0x14) + off, wp);
	}
	int32_t mask = 0;
	for (int32_t i = 0; i < env->flagsCount; ++i)
	{
		if (static_cast<int32_t>(cpRd(env->flagsPtr + 4u * i)) == 1)
		{
			mask = static_cast<int32_t>(1u << (i & 0x1F));
			break;
		}
	}
	cpWr(cpRd(owner + 0x14) + 0x2C, static_cast<uint32_t>(mask));
	cpWr(cpRd(owner + 0x14) + 0x4C, 0);
}
