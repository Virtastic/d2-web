// Batch b68 (NAMED lane). Skipped as Win32/CRT/x87/SEH: 0x58C0 0x62AA 0x6DB8 0x6F53 0x70E7 0x8221 0x8A1D 0x8C14 0x86337 0x868C8.
#include <cstdint>
#include <cstring>
#include <initializer_list>

// Hidden register inputs are explicit leading params. Callee thunks are passed in an env.
struct B68Rng { uint32_t lo, hi; };	// ctx+0x1C4 / +0x1C8 (64-bit LCG state, little endian)

static uint32_t b68Step(uint8_t* ctx)
{
	uint32_t lo, hi;
	memcpy(&lo, ctx + 0x1C4, 4);
	memcpy(&hi, ctx + 0x1C8, 4);
	uint64_t v = (uint64_t)lo * 0x6AC690C5u + hi;
	memcpy(ctx + 0x1C4, &v, 8);
	return (uint32_t)v;
}

// callee 0x106B0: __fastcall(ecx, const int* table, ctx(esi), uint32_t* pLocal)
typedef void (*B68Fn106B0)(uint32_t ecx, const int32_t* pEntry, uint8_t* ctx, uint32_t* pLocal);

// 1.13c D2Common+0x11170  (esi = ctx, passed explicitly; ecx = ctx+0x1C8 value for first call)
extern "C" void D2Common_11170(uint8_t* ctx, const uint8_t* tab6188, const uint8_t* tab61C8, B68Fn106B0 fn)
{
	uint32_t hiBefore;
	memcpy(&hiBefore, ctx + 0x1C8, 4);
	uint32_t local = b68Step(ctx) & 3;
	fn(hiBefore, (const int32_t*)(tab6188 + local * 0x10), ctx, &local);
	int32_t type;
	memcpy(&type, ctx + 0x1D0, 4);
	if (type == 0x81)
		fn(0, (const int32_t*)(tab61C8 + local * 0x10), ctx, &local); // ecx here is an extraout (leftover) register
}

// 1.13c D2Common+0x116B0 (esi = ctx). Sub-calls 0xF000/0x10A00/0x10820/0xE490 go through env; the
// DRLG-room builders' ECX/EAX hidden inputs are not modelled (extraout ECX forwarded as 0).
struct B68Env116B0
{
	int32_t* (*getRoomA)(void);					// 0x10A00 (eax = ctx+0x10 owner)
	int32_t* (*getRoomB)(uint8_t* ctx);			// 0x10820
	int32_t* (*step)(int32_t* p, uint32_t a, int32_t b);	// 0xF000
	void (*mark)(int32_t one);					// 0xE490
	B68Fn106B0 fn;								// 0x106B0
	const uint8_t* t6748; const uint8_t* t6788; const uint8_t* t67C8; const uint8_t* t6808; const uint8_t* t6848;
};

static void b68Tag(int32_t* p, uint32_t id)
{
	// p[8] -> record: [0]=id, [1]=0, [3]|=2
	uint32_t* r = (uint32_t*)(uintptr_t)(uint32_t)p[8];
	r[0] = id; r[1] = 0; r[3] |= 2;
}

extern "C" void D2Common_116B0(uint8_t* ctx, const B68Env116B0* e)
{
	uint32_t local4 = ((b68Step(ctx) & 1) << 1) | 1;
	uint32_t local8 = b68Step(ctx) & 3;
	int32_t type;
	memcpy(&type, ctx + 0x1D0, 4);
	switch (type)
	{
	case 0x2f:
	{
		int32_t* p = e->getRoomA();
		for (int i = 0; i < 2; i++)
		{
			p = e->step(p, 1, 1);
			if (p) { e->mark(1); e->mark(1); }
		}
		p = e->step(p, 0, 0);
		if (p) { e->mark(1); b68Tag(p, 0x14d); }
		p = e->getRoomB(ctx);
		for (int i = 0; i < 2; i++)
		{
			p = e->step(p, 2, 1);
			if (p) { e->mark(1); e->mark(1); }
		}
		p = e->step(p, local4, 0);
		if (p) { e->mark(1); b68Tag(p, 0x150); }
		p = e->step(p, local4, 0);
		if (p) e->mark(1);
		e->fn(0, (const int32_t*)(e->t6788 + local8 * 0x10), ctx, &local8);
		return;
	}
	case 0x30:
		e->fn((uint32_t)(uintptr_t)&local8, (const int32_t*)(e->t6748 + local8 * 0x10), ctx, &local8);
		e->fn(0, (const int32_t*)(e->t67C8 + local8 * 0x10), ctx, &local8);
		e->fn(0, (const int32_t*)(e->t6788 + local8 * 0x10), ctx, &local8);
		return;
	case 0x31:
		e->fn((uint32_t)(uintptr_t)&local8, (const int32_t*)(e->t6748 + local8 * 0x10), ctx, &local8);
		e->fn(0, (const int32_t*)(e->t6808 + local8 * 0x10), ctx, &local8);
		return;
	case 0x41:
		e->fn((uint32_t)(uintptr_t)&local8, (const int32_t*)(e->t6748 + local8 * 0x10), ctx, &local8);
		e->fn(0, (const int32_t*)(e->t6848 + local8 * 0x10), ctx, &local8);
		return;
	}
}

extern "C" __attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
extern "C" __attribute__((weak)) void Ordinal_10024(const void*, uint32_t);

// 1.13c D2Common+0x19950 (eax = unit/owner whose +0x90 heads a chain; ecx unused; edx = param_2 id).
// Chain node: [0]=id, [9..]=payload (returned as node+9 dwords), [0x11]=next. pBase/count: DAT_6fdf0b38 table, sgptDataTables+0xC5C.
extern "C" int32_t* D2Common_19950(uint8_t* eax, int32_t id, uint8_t* pTable, int32_t tableCount, const void* pAssertMsg)
{
	int32_t* n = (int32_t*)(uintptr_t) * (uint32_t*)(eax + 0x90);
	for (;;)
	{
		if (!n)
		{
			if (id >= 0 && id < tableCount)
				return (int32_t*)(pTable + id * 0x9c + 0x68);
			return (int32_t*)0x68;
		}
		if (*n == 0)
		{
			Ordinal_10024(pAssertMsg, Ordinal_10265(0x1a2));
			__builtin_trap(); // retail: _exit(-1)
		}
		if (id == *n) break;
		n = (int32_t*)(uintptr_t)(uint32_t)n[0x11];
	}
	return n + 9;
}
