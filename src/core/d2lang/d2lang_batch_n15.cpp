// 1.13c D2Lang implementation batch 15: MSVC 2003 CRT errno / small-heap helpers.
#include <cstddef>
#include <cstdint>
#include <cstdlib>

static int g_d2lang_errno;
static unsigned long g_d2lang_doserrno;

// 1.13c D2Lang+0x4F4E: _errno() -- address of the per-thread errno slot (one thread here).
extern "C" int* d2lang_crt_errno() { return &g_d2lang_errno; }

// 1.13c D2Lang+0x4F57: __doserrno() -- address of the per-thread DOS error slot.
extern "C" unsigned long* d2lang_crt_doserrno() { return &g_d2lang_doserrno; }

struct D2LangSbhRegion { uint32_t a, b, c, base, e; }; // 0x14 bytes; base at +0xC

// 1.13c D2Lang+0x5FCF: ___sbh_find_block -- region whose 1 MiB span holds p, else null.
extern "C" __attribute__((weak)) D2LangSbhRegion* d2lang_crt_sbh_find_block(D2LangSbhRegion* regions, int count, uint32_t p)
{
	for (D2LangSbhRegion* r = regions; r < regions + count; ++r)
		if (p - r->base < 0x100000u)
			return r;
	return nullptr;
}

// 1.13c D2Lang+0x43B3: __nh_malloc -- malloc; on failure, if nh set, call the new-handler and retry while it returns nonzero.
extern "C" void* d2lang_crt_nh_malloc(size_t size, int nh, int (*newh)(size_t))
{
	if (size >= 0xffffffe1u)
		return nullptr;
	for (;;)
	{
		if (void* p = malloc(size))
			return p;
		if (!nh || !newh || !newh(size))
			return nullptr;
	}
}
