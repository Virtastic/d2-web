// 1.13c D2Lang implementation batch b59-lan-a: MSVC 2003 CRT helpers.
// Per-thread CRT data (__getptd) is one thread here: errno/doserrno are single words.
#include <cstddef>
#include <cstdint>

static int g_crt_errno;
static unsigned long g_crt_doserrno;

// 1.13c D2Lang+0x04F4E: &__getptd()->_terrno.
extern "C" int* d2lang_crt_errno_ptr() { return &g_crt_errno; }

// 1.13c D2Lang+0x04F57: &__getptd()->_tdoserrno.
extern "C" unsigned long* d2lang_crt_doserrno_ptr() { return &g_crt_doserrno; }

// Small-block-heap region header, 0x14 bytes; only the base (+0xc) is read here.
struct D2LangSbhRegion { uint32_t w0, w1, w2, base, w4; };

// 1.13c D2Lang+0x05FCF: ___sbh_find_block -- region (of `count` at `regions`) whose 1 MiB
// span contains addr, else null.
extern "C" D2LangSbhRegion* d2lang_crt_sbh_find_block(D2LangSbhRegion* regions, int count, uint32_t addr)
{
	for (D2LangSbhRegion* r = regions; r < regions + count; ++r)
		if (addr - r->base < 0x100000u) return r;
	return nullptr;
}

// 1.13c D2Lang+0x043B3: __nh_malloc -- retry __heap_alloc, calling the new-handler (callnewh)
// while it returns nonzero and nh_flag is set. Seams are the module's __heap_alloc/__callnewh.
extern "C" void* d2lang_crt_nh_malloc(size_t size, int nh_flag, void* (*heap_alloc)(size_t), int (*callnewh)(size_t))
{
	if (size < 0xffffffe1u) {
		do {
			void* p = heap_alloc(size);
			if (p) return p;
			if (nh_flag == 0) return nullptr;
		} while (callnewh(size) != 0);
	}
	return nullptr;
}
