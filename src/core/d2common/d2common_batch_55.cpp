// 1.13c D2Common implementation batch 55: +0x47350, +0x53C80, +0x5DD80, +0x874E6, +0x87549.
// Bodies from the pinned 1.13c decompiles; retail offsets kept, hidden register args made explicit,
// UNMAPPED callees and retail globals reached through env structs / function pointers.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace
{
	inline uint32_t Rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }
}

// 1.13c D2Common+0x47350: clamp a 3-int record (+4 -> 0..3, +8/+0xC biased by 0x100 -> 0..0x3FF)
// and, for a valid record index below the table count (sgptDataTables+0xBD4, base +0xBCC, stride 0x144),
// resolve a slot through 0x47020 and store (index, clampedA + (clampedY*0x400 + clampedX)*4) via 0x3A280.
// Returns the clamped +0xC value on success, else 0. EDX = pUnit(param_2); param_1/3/5/7/8 unused.
struct D2Common47350Env
{
	uint32_t pDataTables;                                       // *sgptDataTables
	uint32_t (*fn47020)(uint32_t p2, uint32_t p9, uint32_t p10); // 0x47020
	void (*fn3A280)(uint32_t p, int32_t idx, int32_t v);         // 0x3A280
};

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_47350(uint32_t param_2, uint32_t param_4, int16_t param_6,
                                                   uint32_t param_9, uint32_t param_10, const D2Common47350Env* env)
{
	if (!param_4 || !env)
		return 0;
	const int32_t i = param_6;
	if (i < 0 || i >= static_cast<int32_t>(Rd32(env->pDataTables + 0xBD4)) ||
	    static_cast<uint32_t>(i) * 0x144 + Rd32(env->pDataTables + 0xBCC) == 0)
		return 0;
	int32_t a = static_cast<int32_t>(Rd32(param_4 + 4));
	int32_t x = static_cast<int32_t>(Rd32(param_4 + 8)) + 0x100;
	int32_t y = static_cast<int32_t>(Rd32(param_4 + 0xC)) + 0x100;
	a = a < 1 ? 0 : (a > 2 ? 3 : a);
	x = x < 1 ? 0 : (x > 0x3FE ? 0x3FF : x);
	y = y < 1 ? 0 : (y > 0x3FE ? 0x3FF : y);
	const uint32_t p = env->fn47020(param_2, param_9, param_10);
	if (!p)
		return 0;
	env->fn3A280(p, i, a + (y * 0x400 + x) * 4);
	return y;
}

// 1.13c D2Common+0x53C80: parse a comma list of 1..3 char tokens (quotes stripped) into
// record+0x26+slot*12 (12 code bytes, preset 0xFF) and the count at record+0x15+slot. Each token is
// blank-padded to a 4-char tag and mapped through Ordinal_10213(table 6FDEFF58, tag, 1). Tokens of
// 5+ chars end the list; 12 tokens max. For slot 15 the counts 0x15..0x24 are summed as (n-1 for
// n>1), capped 0xFE, into record+0x25. EDX/ECX: param_1 text (modified in place), param_2 record.
struct D2Common53C80Env
{
	uint8_t (*lookup)(uint32_t tag);  // Ordinal_10213(DAT_6fdeff58, tag, 1)
};

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_53c80(char* p, uint8_t* rec, int32_t slot, const D2Common53C80Env* env)
{
	if (!rec || slot < 0 || slot >= 16 || !env)
		return;
	rec[0x15 + slot] = 0;
	uint8_t* codes = rec + 0x26 + slot * 12;
	memset(codes, 0xFF, 12);
	if (!p)
		return;
	if (*p == '"')
		++p;
	for (char* q = p; *q; ++q)
		if (*q == '"')
			*q = 0;
	int32_t n = 0;
	for (;;)
	{
		char* next = strchr(p, ',');
		if (next)
			*next++ = 0;
		if (!*p || strlen(p) > 4)
			break;
		char t[4] = {p[0], ' ', ' ', ' '};
		if (p[1])
		{
			t[1] = p[1];
			if (p[2])
			{
				t[2] = p[2];
				if (p[3])
					t[3] = p[3];
			}
		}
		uint32_t tag;
		memcpy(&tag, t, 4);
		codes[n++] = env->lookup(tag);
		if (!next || n > 0xB)
			break;
		p = next;
	}
	rec[0x15 + slot] = static_cast<uint8_t>(n);
	if (slot == 0xF)
	{
		int32_t sum = 0;
		for (int32_t k = 0; k < 16; ++k)
			if (rec[0x15 + k] > 1)
				sum += rec[0x15 + k] - 1;
		rec[0x25] = static_cast<uint8_t>(sum > 0xFE ? 0xFE : sum);
	}
}

// 1.13c D2Common+0x5DD80: append param_2 bytes from param_1 to a growable buffer. IN: EAX = capacity
// int*, ECX = buffer pointer slot, EBX = used-size int*. While capacity <= used + n the capacity grows
// by 0x400 (fail, return 0, once it exceeds 0x7FFFFFFE) and the buffer is re-allocated with
// Ordinal_10047(cap, "DataTbls.cpp", 0x292, 0) WITHOUT copying the old contents (as retail). Returns
// the pre-append offset.
struct D2Common5DD80Env
{
	uint32_t (*alloc)(int32_t size, int32_t line); // Ordinal_10047(size, DataTbls.cpp, line, 0)
};

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_5dd80(int32_t* pCap, uint32_t* ppBuf, int32_t* pUsed,
                                                   const uint8_t* src, uint32_t n, const D2Common5DD80Env* env)
{
	while (*pCap <= static_cast<int32_t>(*pUsed + n))
	{
		*pCap += 0x400;
		if (*pCap > 0x7FFFFFFE)
			return 0;
		*ppBuf = env->alloc(*pCap, 0x292);
	}
	const int32_t off = *pUsed;
	memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(*ppBuf + off)), src, n);
	*pUsed += n;
	return off;
}

extern "C" {
int32_t d2common_872d4(uint32_t* param_1, int32_t param_2, int32_t param_3, int32_t param_4);  // 0x872D4
void d2_fn_d2common_87447(const uint32_t* p1, char* p2, uint32_t p3, int32_t p4);             // 0x87447
char* d2_fn_d2common_873e0(const uint32_t* p1, char* p2, uint32_t p3);                        // 0x873E0
}

// 1.13c D2Common+0x874E6: MSVC CRT _cfltcvt dispatcher. fmt 'e'/'E' -> 0x872D4, 'f' -> 0x873E0
// (its result is returned), anything else -> 0x87447.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2common_cfltcvt_874e6(double* arg, char* buf, int32_t fmt, int32_t prec, int32_t caps)
{
	uint32_t* a = reinterpret_cast<uint32_t*>(arg);
	if (fmt == 0x65 || fmt == 0x45)
		return d2common_872d4(a, static_cast<int32_t>(reinterpret_cast<uintptr_t>(buf)), prec, caps);
	if (fmt == 0x66)
		return static_cast<int32_t>(reinterpret_cast<uintptr_t>(d2_fn_d2common_873e0(a, buf, static_cast<uint32_t>(prec))));
	d2_fn_d2common_87447(a, buf, static_cast<uint32_t>(prec), caps);
	return 0;
}

// 1.13c D2Common+0x87549: CRT __ms_p5_test_fdiv (Pentium FDIV probe). Constants at 6FDE6F48/50/58
// are 1.0, 4195835.0, 3145727.0 (read from the 1.13c image).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2common_ms_p5_test_fdiv_87549(void)
{
	volatile double a = 1.0, b = 4195835.0, c = 3145727.0;
	return a < b - (b / c) * c ? 1 : 0;
}
