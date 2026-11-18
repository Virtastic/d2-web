// The C runtime that retail D2Common.dll links statically (VS2003 CRT), written from the 1.13c decompile: the d2moo tree has no
// 1.10f counterpart, because 1.10f called the system CRT. Every function carries its 1.13c address. The lock, atexit and locale
// functions take their callees (allocator, critical sections, errno, the locale mapper) as a table of function pointers: those
// callees are host services, not game code.
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <emscripten.h>

namespace
{
// Byte loads avoid alignment/aliasing UB while retaining the x86 instruction's read-all-four-before-writing behaviour, even
// for one-byte overlaps. Volatile prevents the compiler replacing this implementation with a host CRT call.
uint32_t D2CommonCrt_LoadWord(const volatile uint8_t* p)
{
	return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}

void D2CommonCrt_StoreWord(volatile uint8_t* p, uint32_t word)
{
	p[0] = uint8_t(word); p[1] = uint8_t(word >> 8);
	p[2] = uint8_t(word >> 16); p[3] = uint8_t(word >> 24);
}

uint8_t D2CommonCrt_AsciiLower(uint8_t value)
{
	return value >= 'A' && value <= 'Z' ? uint8_t(value + 32) : value;
}
}

// 1.13c D2Common+0x2320 _memmove (cdecl): returns the original destination; the byte count stays 32-bit. Retail's <=7-word
// fallthrough and rep-movsd paths do the same ordered word transfers, expressed here as one loop.
extern "C" void* D2Common_02320_Memmove(void* destination, const void* source, uint32_t count)
{
	auto* dst = static_cast<volatile uint8_t*>(destination);
	auto* src = static_cast<const volatile uint8_t*>(source);
	const uintptr_t d = reinterpret_cast<uintptr_t>(destination);
	const uintptr_t s = reinterpret_cast<uintptr_t>(source);
	// Unsigned distance avoids overflow in source+count, and does not truncate wasm64 pointers to the retail 32-bit space.
	if (s < d && d - s < count)
	{
		uint32_t remaining = count;
		if (remaining >= 4)
		{
			uint32_t tail = uint32_t((d + remaining) & 3);
			while (tail--) { --remaining; dst[remaining] = src[remaining]; }
			while (remaining >= 4)
			{
				remaining -= 4;
				const uint32_t word = D2CommonCrt_LoadWord(src + remaining);
				D2CommonCrt_StoreWord(dst + remaining, word);
			}
		}
		while (remaining) { --remaining; dst[remaining] = src[remaining]; }
	}
	else
	{
		uint32_t offset = 0;
		uint32_t remaining = count;
		if (remaining >= 4)
		{
			uint32_t head = uint32_t((0 - d) & 3);
			while (head--) { dst[offset] = src[offset]; ++offset; --remaining; }
			while (remaining >= 4)
			{
				const uint32_t word = D2CommonCrt_LoadWord(src + offset);
				D2CommonCrt_StoreWord(dst + offset, word);
				offset += 4; remaining -= 4;
			}
		}
		while (remaining--) { dst[offset] = src[offset]; ++offset; }
	}
	return destination;
}

// 1.13c D2Common+0x1F81 printf: narrow printf wrapper, despite Ghidra's conflicting _wprintf library label. Host stdio owns
// the stdout lock and buffering; its FILE layout must never be substituted for the retail 32-byte FILE at DLL+0x99328.
extern "C" int D2Common_1F81_Printf(const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	const int result = std::vprintf(format, arguments);
	va_end(arguments);
	return result;
}

// 1.13c D2Common+0x26A0 _strcspn(str, control): builds a 256-bit set of the bytes in control (terminator excluded) and
// returns the count of leading str bytes not in it.
uint32_t d2common_strcspn_26a0(const char* str, const char* control)
{
	uint8_t set[32] = {};
	for (; *control; ++control)
	{
		uint8_t c = (uint8_t)*control;
		set[c >> 3] |= (uint8_t)(1u << (c & 7));
	}
	uint32_t n = 0;
	for (; *str; ++str, ++n)
	{
		uint8_t c = (uint8_t)*str;
		if ((set[c >> 3] >> (c & 7)) & 1)
		{
			break;
		}
	}
	return n;
}

// 1.13c D2Common+0x1EC0 _stricmp. The game uses the initial C locale. A non-null mapping represents the already resolved CRT
// locale's byte-to-lowercase results; no host locale state is read.
extern "C" int32_t D2Common_Stricmp_1ec0(const char* left, const char* right, const int32_t* lowercase)
{
	for (;;)
	{
		const auto a = static_cast<uint8_t>(*left++);
		const auto b = static_cast<uint8_t>(*right++);
		const int32_t foldedA = lowercase ? lowercase[a] : D2CommonCrt_AsciiLower(a);
		const int32_t foldedB = lowercase ? lowercase[b] : D2CommonCrt_AsciiLower(b);
		if (foldedA != foldedB)
		{
			return lowercase ? foldedA - foldedB : (foldedA < foldedB ? -1 : 1);
		}
		if (foldedA == 0)
		{
			return 0;
		}
	}
}

// 1.13c D2Common+0x26F0 _strncpy: copies up to count bytes, zero-padding after the terminator.
extern "C" char* D2Common_Strncpy_26f0(char* dest, const char* src, uint32_t count)
{
	char* const result = dest;
	while (count != 0)
	{
		const char value = *src++;
		*dest++ = value;
		--count;
		if (value == '\0')
		{
			while (count != 0)
			{
				*dest++ = '\0';
				--count;
			}
			break;
		}
	}
	return result;
}

typedef int32_t (*D2CommonIsctypeFn)(const uint8_t* loc, uint32_t c, int32_t mask);
typedef int32_t (*D2CommonLcMapFn)(uint32_t lcid, uint32_t flags, const char* src, int32_t srcLen, uint8_t* out, int32_t outLen, uint32_t codePage, int32_t);

// 1.13c D2Common+0x2A24 ___tolower_mt(locinfo, c) (VS2003 CRT). locinfo fields: +04 code page, +14 LCID, +24 C-locale-ctype
// flag, +28 mb_cur_max, +48 ctype table. Callees are seams: ___isctype_mt (+0x31F9) and the LCMapString wrapper (+0x4EF4); the
// latter writes up to 3 bytes into out.
extern "C" uint32_t D2Common_Tolower_mt_2a24(const uint8_t* loc, uint32_t c, D2CommonIsctypeFn isctype, D2CommonLcMapFn lcmap)
{
	auto u32 = [&](int o) { return *reinterpret_cast<const uint32_t*>(loc + o); };
	if (u32(0x14) == 0 || (u32(0x24) != 0 && c < 0x80))
	{
		return (int32_t(c) > 0x40 && int32_t(c) < 0x5b) ? c + 0x20 : c;
	}
	const uint8_t* ctype = reinterpret_cast<const uint8_t*>(uintptr_t(u32(0x48)));
	if (c < 0x100)
	{
		const uint32_t upper = int32_t(u32(0x28)) < 2 ? (ctype[c * 2] & 1) : uint32_t(isctype(loc, c, 1));
		if (!upper)
		{
			return c;
		}
	}
	char in[4] = {};
	int32_t n;
	if ((ctype[1 + ((c >> 8) & 0xff) * 2] & 0x80) == 0) { in[0] = char(c); n = 1; }
	else { in[0] = char(c >> 8); in[1] = char(c); n = 2; }
	uint8_t out[4] = {};
	const int32_t r = lcmap(u32(0x14), 0x100, in, n, out, 3, u32(4), 1);
	if (r == 0)
	{
		return c;
	}
	if (r != 1)
	{
		return (uint32_t(out[0]) << 8) | out[1];
	}
	return out[0];
}

extern "C" {
int32_t d2common_incman_875e4(uint32_t*, int32_t);

// 1.13c D2Common+0x875B2 __ZeroTail: 1 when the 3-word mantissa is zero below the given bit.
EMSCRIPTEN_KEEPALIVE int32_t d2common_zerotail_875b2(const uint32_t* m, int32_t bit)
{
	int32_t i = bit / 32;
	uint32_t v = m[i] & ~(~0u << ((31u - (uint32_t)(bit % 32)) & 31));
	for (;;)
	{
		if (v) return 0;
		if (++i > 2) return 1;
		v = m[i];
	}
}

// 1.13c D2Common+0x87631 __RoundMan: rounds the 3-word mantissa at the given bit (__ZeroTail / __IncMan are +0x875B2 / +0x875E4).
EMSCRIPTEN_KEEPALIVE int32_t d2common_roundman_87631(uint32_t* m, int32_t bit)
{
	int32_t r = 0, i = bit / 32;
	uint32_t sh = (31u - (uint32_t)(bit % 32)) & 31;
	if ((m[i] & (1u << sh)) && !d2common_zerotail_875b2(m, bit)) r = d2common_incman_875e4(m, bit - 1);
	m[i] &= ~0u << sh;
	for (i++; i < 3; i++) m[i] = 0;
	return r;
}

// 1.13c D2Common+0x876D7 __ShrMan: 3-word mantissa right shift by n bits.
EMSCRIPTEN_KEEPALIVE void D2Common_ShrMan_876D7(uint32_t* m, int32_t n)
{
	uint32_t b = (uint32_t)(n % 32), carry = 0;
	for (int i = 0; i < 3; i++)
	{
		uint32_t v = m[i];
		m[i] = (v >> (b & 31)) | carry;
		carry = (v & ~(~0u << (b & 31))) << ((32u - b) & 31);
	}
	int32_t w = n / 32;
	for (int i = 2; i >= 0; i--) m[i] = i < w ? 0 : m[2 - w - (2 - i)];
}
}

// 1.13c D2Common+0x70F40 (ordinal 10352): a stdcall no-op that discards its one stack word; EAX on return is EAX on entry.
extern "C" uint32_t d2common_70f40(uint32_t eax, uint32_t /*discarded*/)
{
	return eax;
}

struct D2CommonMtLocksEnv
{
	int32_t (*init_cs_spin)(void* cs, uint32_t spin);	// ___crtInitCritSecAndSpinCount (0x5E4D)
};

// 1.13c D2Common+0x2DA6 __mtinitlocks (73 B): table DAT_6fde9048, 0x24 records of 8 bytes {cs ptr, preallocate flag}.
// Each record whose flag (+4) is 1 gets the next 0x18-byte slice of the static pool DAT_6fdef600 as its critical section and
// it is initialised with spin 4000; on failure that record's pointer is cleared and 0 returned. Returns 1 when all are set.
extern "C" int32_t D2Common_2DA6_MtInitLocks(uint32_t (*tbl)[2], uint8_t* pool, const D2CommonMtLocksEnv* env)
{
	for (int i = 0; i < 0x24; i++)
	{
		if (tbl[i][1] == 1)
		{
			tbl[i][0] = (uint32_t)(uintptr_t)pool;
			pool += 0x18;
			if (env->init_cs_spin(pool - 0x18, 4000) == 0) { tbl[i][0] = 0; return 0; }
		}
	}
	return 1;
}

struct D2CommonMtDelEnv
{
	void (*delete_cs)(void* cs);	// DeleteCriticalSection
	void (*free_)(void* p);			// _free (0x4C0D)
};

// 1.13c D2Common+0x2DEF __mtdeletelocks (85 B): the lock table (DAT_6fde9048..0x6fde9168, 36 entries of {cs*, flag}, 8-byte
// stride). Pass 1: every non-null entry whose flag != 1 is deleted, freed and nulled. Pass 2: every non-null entry whose
// flag == 1 (statically owned) is only deleted, not freed or nulled.
extern "C" void D2Common_2DEF_MtDeleteLocks(uint32_t (*table)[2], const D2CommonMtDelEnv* env)
{
	for (int i = 0; i < 36; i++)
	{
		if (table[i][0] && table[i][1] != 1)
		{
			void* cs = (void*)(uintptr_t)table[i][0];
			env->delete_cs(cs);
			env->free_(cs);
			table[i][0] = 0;
		}
	}
	for (int i = 0; i < 36; i++)
	{
		if (table[i][0] && table[i][1] == 1)
		{
			env->delete_cs((void*)(uintptr_t)table[i][0]);
		}
	}
}

struct D2CommonMtLockEnv
{
	void* (*malloc_)(uint32_t n);						// 0x4C0D-family _malloc
	void (*free_)(void* p);								// _free
	int32_t* (*errno_ptr)();							// 0x44BC
	void (*lock10)();									// __lock(10)  (0x2EF9)
	void (*unlock10)();									// 0x2EF0 _unlock(10)
	int32_t (*init_cs_spin)(void* cs, uint32_t spin);	// ___crtInitCritSecAndSpinCount
};

// 1.13c D2Common+0x2E59 __mtinitlocknum (151 B): make sure lock-table slot `slot` (DAT_6fde9048, 8-byte stride; the dword at
// the slot is the critical-section pointer) holds an allocated 0x18-byte critical section. Allocate first; on failure set
// errno=12 and return 0. Under lock 10 re-check the slot (another thread may have filled it): if still empty init the section
// with spin 4000 (failure: free, errno=12, unlock via the SEH unwind, return 0) and store it; otherwise free the spare.
// Unlock 10, return 1.
extern "C" int32_t D2Common_2E59_MtInitLockNum(void** slot, const D2CommonMtLockEnv* env)
{
	if (*slot != nullptr)
	{
		return 1;
	}
	void* cs = env->malloc_(0x18);
	if (!cs)
	{
		*env->errno_ptr() = 12;
		return 0;
	}
	env->lock10();
	if (*slot == nullptr)
	{
		if (env->init_cs_spin(cs, 4000) == 0)
		{
			env->free_(cs);
			*env->errno_ptr() = 12;
			env->unlock10();	// __local_unwind2 runs the __finally -> _unlock(10)
			return 0;
		}
		*slot = cs;
	}
	else
	{
		env->free_(cs);
	}
	env->unlock10();
	return 1;
}

struct D2CommonOnexitInitEnv
{
	uint32_t* tableBase;	// DAT 0x6fdf2854
	uint32_t* tableEnd;		// DAT 0x6fdf2850
	uint32_t* (*malloc_)(uint32_t);
};

// 1.13c D2Common+0x2FAA ___onexitinit(): malloc 0x80 bytes for the atexit table; on failure return 0x18 (_RT_SPACEARG); else
// first slot = 0, both table pointers (DAT 0x6fdf2854 / 0x6fdf2850) = block; return 0.
uint32_t d2common_onexitinit_2faa(D2CommonOnexitInitEnv& e)
{
	e.tableBase = e.malloc_(0x80);
	if (!e.tableBase)
	{
		return 0x18;
	}
	*e.tableBase = 0;
	e.tableEnd = e.tableBase;
	return 0;
}
