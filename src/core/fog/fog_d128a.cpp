// Fog log writer, pool create, pool alloc, AllocClientMemory (d128a): Fog+0x18770, +0x1AE90,
// +0x1B560, +0x1CD10 as real extern "C" with the hidden EAX/ECX/EBX/ESI inputs as explicit params.
// Retail 32-bit node/pool layout: node and pool fields store the LOW 32 BITS of the wasm address and are
// widened with uintptr_t on read (same convention as storm_sub_51f0.cpp; the wasm heap stays under 4GB).
// Win32 calls (Enter/Leave/InitializeCriticalSection, GetLocalTime, RaiseException) stay seams.
// Prior art: reference/D2MOO/source/Fog (names only); layout recovered from the decompiles.
//
// The pool state is one 0xBEA4-byte block (retail DAT_6ff9af78..): +0 init flag, +4 global CS,
// +0x1c eight pools of 0x17cc bytes, +0xBE7C pools used, +0xBE80 free-pool stack, +0xBEA0 its count.
// A pool: +4 CS, +0x1c nClasses, +0x20 classes of 0x30 bytes, +0x7a0 block count, +0x7a4 capacity,
// +0x7a8 block table, +0x7ac 0x3ff-bucket hash, +0x17ac name. A class: +0 CS, +0x18 element size,
// +0x1c elements per block, +0x20 block bytes, +0x24 block count, +0x28 first block, +0x2c current.
// A block: +0 data, +4 bitmap, +8 used, +0xc prev, +0x10 next, +0x14 class.
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>

extern "C" {
void* FUN_6ff51290(void* root, int32_t size, uint32_t flags, int32_t mode);   // Fog+0x1290
void fog_sub_11690(uint32_t*);
void FUN_6ff5fc20(void);
void Ordinal_116(void);
void FUN_6ff60f30(int, const char*, int, const char*);
void FUN_6ff684a0(void);
uint32_t* FUN_6ff6b190(int32_t size, uint8_t* mgr);                           // Fog+0x1B190
void FUN_6ff6ad20(const char* fmt, ...);                                      // Fog+0x1AD20
void Ordinal_501(char* dst, const char* src, uint32_t cap);                   // Storm SStrCopy
extern const char DAT_6ff768ec[];
extern uint32_t g_fog_819a0, g_fog_818f8;                                      // fog_sub_19530.cpp
}
int fog_sub_1abb0(uint32_t, const uint32_t*);                                 // Fog+0x1ABB0

// Storm #506 / #503, weak so a real Storm shim wins.
extern "C" __attribute__((weak)) size_t Ordinal_506(const char* s) { return strlen(s); }
extern "C" __attribute__((weak)) int Ordinal_503(char* base, const char* add, int max) {
	size_t n = strlen(base);
	if ((int)n + 1 >= max) return (int)n;
	size_t k = strlen(add);
	if (n + k + 1 > (size_t)max) k = max - n - 1;
	memcpy(base + n, add, k); base[n + k] = 0;
	return (int)(n + k);
}

namespace
{
	inline uint32_t W32(const void* p) { return (uint32_t)(uintptr_t)p; }
	inline uint32_t* P32(uint32_t v) { return (uint32_t*)(uintptr_t)v; }
	inline uint8_t* PB(uint32_t v) { return (uint8_t*)(uintptr_t)v; }
	inline uint32_t& U(uint8_t* b, uint32_t off) { return *(uint32_t*)(b + off); }

	[[noreturn]] void fog_fatal(int line)   // FC20, Ordinal_116, 10F30(6,"",line,..), 184A0, _exit(-1)
	{
		FUN_6ff5fc20(); Ordinal_116();
		FUN_6ff60f30(6, "", line, "Unrecoverable internal error %08x");
		FUN_6ff684a0();
		_Exit(-1);
	}

	// log list root (retail DAT_6ffa6ebc): +0 base, +4 sentinel pNext, +8 sentinel nPrev.
	uint32_t g_log_root[3];
	uint32_t g_log_count, g_log_bytes;       // DAT_6ff816f0, DAT_6ff81848
	CRITICAL_SECTION g_log_cs;               // DAT_6ffa6e8c
	struct LogRootInit { LogRootInit() {     // Fog+0x1120 list-root constructor
		g_log_root[1] = W32(&g_log_root[1]); g_log_root[0] = 0; g_log_root[2] = ~W32(&g_log_root[1]); } } g_log_root_init;

	alignas(8) uint8_t g_pools[0xBEA4];      // DAT_6ff9af78..
	uint8_t* const POOL0 = g_pools + 0x1c;
	int bsr32(uint32_t v) { return 31 - __builtin_clz(v); }
	int class_of(int32_t n)                  // size -> class index, as in 1AE90/1B560
	{
		if (n < 0x11) return 0;
		uint32_t u = (uint32_t)n - 1;
		return (u == 0 ? 0 : bsr32(u) + 1) - 4;
	}
}

// 1.13c Fog+0x18770 (Fog log writer; unnamed Fog-internal function): format, append a newline when
// p3 == 1, and push a timestamped message node on the log list.
extern "C" EMSCRIPTEN_KEEPALIVE void FUN_6ff68770(const char* ch, int p2, int p3, const char* fmt, va_list ap)
{
	if (g_fog_819a0 != 0 || g_fog_818f8 == 0) return;
	char buf[0x1000];   // retail local is 4092 bytes with vsnprintf(.., 0x1000): the 4 extra are slack here
	vsnprintf(buf, sizeof buf, fmt, ap);
	if (p3 == 1) Ordinal_503(buf, "\n", 0x1000);
	int n = (int)Ordinal_506(ch) + 2 + (int)Ordinal_506(buf);
	EnterCriticalSection(&g_log_cs);
	uint8_t* m = (uint8_t*)FUN_6ff51290(g_log_root, n, 0, 0);
	LeaveCriticalSection(&g_log_cs);
	U(m, 8) = n + 0x24;
	U(m, 12) = 1;
	U(m, 16) = (p2 == 1);
	GetLocalTime((LPSYSTEMTIME)(m + 0x14));
	Ordinal_501((char*)m + 0x24, ch, n);
	int k = (int)Ordinal_506((char*)m + 0x24);
	Ordinal_501((char*)m + k + 0x25, buf, n);
	EnterCriticalSection(&g_log_cs);
	uint32_t nx = U(m, 0);
	if (nx != 0) {   // unlink the node from whatever list it is on
		uint32_t tag = U(m, 4);
		uint32_t* slot = (int32_t)tag < 0 ? P32(~tag) : P32(W32(m) + (tag - P32(nx)[1]));
		*slot = nx;
		P32(U(m, 0))[1] = U(m, 4);
		U(m, 0) = 0; U(m, 4) = 0;
	}
	uint32_t* head = P32(g_log_root[1]);   // DAT_6ffa6ec0
	U(m, 0) = W32(head);
	U(m, 4) = head[1];
	head[1] = W32(m);
	g_log_count++;
	g_log_bytes += U(m, 8);
	g_log_root[1] = W32(m);
	LeaveCriticalSection(&g_log_cs);
}

// 1.13c Fog+0x1AE90 Ordinal 10142: create a pool (param_1 NULL = first-time init of the global state).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t FUN_6ff6ae90(uint32_t* param_1, const char* name, int32_t blockSize)
{
	if (param_1 == 0) {
		memset(g_pools, 0, sizeof g_pools);
		InitializeCriticalSection((LPCRITICAL_SECTION)(g_pools + 4));
		U(g_pools, 0) = 1;
	}
	EnterCriticalSection((LPCRITICAL_SECTION)(g_pools + 4));
	uint32_t& used = U(g_pools, 0xBE7C);
	uint32_t& nfree = U(g_pools, 0xBEA0);
	uint32_t idx;
	if (nfree == 0) {
		if (7 < (int32_t)used) {
			LeaveCriticalSection((LPCRITICAL_SECTION)(g_pools + 4));
			FUN_6ff6ad20("Insufficient pre-allocated pool systems");
			abort();   // seam: RaiseException(0xe0000001) has no wasm handler
			return 0;
		}
		idx = used++;
	} else {
		nfree--;
		idx = U(g_pools, 0xBE80 + nfree * 4);
	}
	uint8_t* pool = POOL0 + idx * 0x17cc;
	LeaveCriticalSection((LPCRITICAL_SECTION)(g_pools + 4));
	memset(pool, 0, 0x17cc);
	if (param_1) *param_1 = W32(pool);
	LPCRITICAL_SECTION cs = (LPCRITICAL_SECTION)(pool + 4);
	InitializeCriticalSection(cs);
	fog_sub_11690((uint32_t*)cs);
	EnterCriticalSection(cs);
	if (name) { Ordinal_501((char*)pool + 0x17ac, name, 0x20); pool[0x17cb] = 0; }
	int nc = (blockSize < 0x11 ? 0 : class_of(blockSize)) + 1;
	if (0x28 < nc) fog_fatal(0x86c);
	U(pool, 0x1c) = nc;
	U(pool, 0x7a8) = 0;
	for (int i = 0; i < nc; i++) {
		uint8_t* c = pool + 0x20 + i * 0x30;
		InitializeCriticalSection((LPCRITICAL_SECTION)c);
		int32_t elem = i == 0 ? 0x10 : 1 << ((i + 4) & 0x1f);
		int32_t units;
		if (elem < 0xf4241 && elem * 0x100 < 0x40000) {
			units = 0x100;
			if (elem * 0x100 < 0x10000) units = 0x10000 / elem;
		} else units = 0x80000 / elem;
		U(c, 0x18) = elem; U(c, 0x1c) = units; U(c, 0x20) = elem * units;
	}
	memset(pool + 0x7ac, 0, 0x3ff * 4);
	fog_sub_11690((uint32_t*)cs);
	LeaveCriticalSection(cs);
	return 0;
}

// Pool block helpers of 1B560 (Fog+0x1B240 / +0x1B390 / +0x1B4F0), over the 32-bit layout; the
// existing rows of those addresses keep native-pointer structs that do not fit the layout above.
static void pool_block_insert(uint8_t* mgr, uint32_t* b)   // register in the key-sorted table
{
	int32_t& cnt = (int32_t&)U(mgr, 0x7a0);
	int32_t& cap = (int32_t&)U(mgr, 0x7a4);
	if (cap <= cnt) {
		cap += 0x20;
		void* t = realloc(PB(U(mgr, 0x7a8)), (size_t)cap * 8)   /* Fog+0x20D7 is the CRT realloc */;
		U(mgr, 0x7a8) = W32(t);
		if (!t) fog_fatal(0x261);
	}
	uint8_t* cls = PB(b[5]);
	U(mgr, 0x17a8) += U(cls, 0x20) + 0x18 + ((U(cls, 0x1c) + 0x1f) >> 5) * 4;
	uint32_t* tab = P32(U(mgr, 0x7a8));
	int lo = 0, hi = cnt;
	while (lo < hi) { int mid = lo + ((hi - lo) >> 1); if (tab[mid * 2 + 1] < b[0]) lo = mid + 1; else hi = mid; }
	if (lo < cnt) memmove(&tab[(lo + 1) * 2], &tab[lo * 2], (size_t)(cnt - lo) * 8);
	tab[lo * 2] = W32(b); tab[lo * 2 + 1] = b[0];
	cnt++;
}

static uint32_t* pool_block_new(uint8_t* mgr, uint8_t* cls)
{
	int32_t& cnt = (int32_t&)U(mgr, 0x7a0);
	int32_t& cap = (int32_t&)U(mgr, 0x7a4);
	if (cap <= cnt) {
		cap += 0x20;
		void* t = realloc(PB(U(mgr, 0x7a8)), (size_t)cap * 8)   /* Fog+0x20D7 is the CRT realloc */;
		U(mgr, 0x7a8) = W32(t);
		if (!t) { FUN_6ff6ad20("(1) Pool Blocks overflowed at %d", cnt); fog_fatal(0x2ff); }
	}
	uint32_t words = (U(cls, 0x1c) + 0x1f) >> 5;
	uint32_t* b = (uint32_t*)malloc(words * 4 + 0x18);
	void* mem = calloc(1, U(cls, 0x20));
	if (!b || !mem) {
		FUN_6ff6ad20("(2) nPoolBlocks = %d/%d  nBlockSize = %d  nUsageTableSize = %d", cnt, cap, U(cls, 0x20), words);
		fog_fatal(800);
	}
	memset(b, 0, 0x18 + words * 4);
	b[1] = W32(b + 6);
	b[5] = W32(cls);
	b[0] = W32(mem);
	U(cls, 0x24)++;
	pool_block_insert(mgr, b);
	return b;
}

// returns the free bit index, block in *out
static int pool_block_pick(uint8_t* mgr, uint8_t* cls, uint32_t** out)
{
	uint32_t* first = P32(U(cls, 0x28));
	if (!first) {
		uint32_t* n = pool_block_new(mgr, cls);
		if (n) { U(cls, 0x2c) = W32(n); U(cls, 0x28) = W32(n); }
		*out = n;
		return 0;
	}
	if ((int32_t)first[2] < (int32_t)U(cls, 0x1c)) {
		int r = fog_sub_1abb0(U(cls, 0x1c), P32(first[1]));
		if (r >= 0) { *out = first; return r; }
	}
	uint32_t* n = pool_block_new(mgr, cls);
	if (n) {
		n[4] = U(cls, 0x28);
		P32(U(cls, 0x28))[3] = W32(n);
		U(cls, 0x28) = W32(n);
	}
	*out = n;
	return 0;
}

// 1.13c Fog+0x1B560: allocate `size` bytes from pool `pMgr` (NULL = the global pool); ECX = pMgr, EAX = size.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* FUN_6ff6b560(int32_t size, uint8_t* pMgr)
{
	uint8_t* mgr = pMgr ? pMgr : POOL0;
	if (U(g_pools, 0) == 0) FUN_6ff6ae90(nullptr, "Global Pool System", 0x40000);
	if (U(mgr, 0) == 0) {
		int c = class_of(size);
		if (c < (int32_t)U(mgr, 0x1c)) {
			uint8_t* cls = mgr + 0x20 + c * 0x30;
			fog_sub_11690((uint32_t*)cls);
			EnterCriticalSection((LPCRITICAL_SECTION)cls);
			uint32_t* b;
			uint32_t bit = (uint32_t)pool_block_pick(mgr, cls, &b);
			if ((int32_t)U(cls, 0x1c) <= (int32_t)bit) fog_fatal(0x4f3);
			if (!b) {
				fog_sub_11690((uint32_t*)cls);
				LeaveCriticalSection((LPCRITICAL_SECTION)cls);
				fog_fatal(0x4f8);
			}
			b[2]++;
			uint32_t* word = P32(b[1]) + ((int32_t)bit >> 5);
			uint32_t mask = 1u << (bit & 0x1f);   // gdwBitMasks[bit & 31]
			if (mask & *word) fog_fatal(0x4ff);
			*word |= mask;
			if ((int32_t)U(cls, 0x1c) <= (int32_t)b[2] && b[4] != 0 &&
			    (int32_t)P32(b[4])[2] < (int32_t)U(cls, 0x1c)) {
				if (b[3] == 0) U(cls, 0x28) = b[4]; else P32(b[3])[4] = b[4];
				if (b[4] != 0) P32(b[4])[3] = b[3];
				if (U(cls, 0x2c) == 0) fog_fatal(0x50d);
				P32(U(cls, 0x2c))[4] = W32(b);
				b[3] = U(cls, 0x2c);
				U(cls, 0x2c) = W32(b);
				b[4] = 0;
			}
			uint32_t r = U(cls, 0x18) * bit + b[0];
			fog_sub_11690((uint32_t*)cls);
			LeaveCriticalSection((LPCRITICAL_SECTION)cls);
			return P32(r);
		}
	}
	return FUN_6ff6b190(size, mgr);
}

// 1.13c Fog+0x1CD10 Ordinal 10042 AllocClientMemory(size, file, line).
extern "C" EMSCRIPTEN_KEEPALIVE
void* Ordinal_10042(int32_t size, const char* file, int32_t line)
{
	void* p = FUN_6ff6b560(size, nullptr);
	if (!p) FUN_6ff60f30(2, file, line, DAT_6ff768ec);
	return p;
}
