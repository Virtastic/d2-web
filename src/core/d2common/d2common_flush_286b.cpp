// __flush (MSVC 2003 CRT): writes pending buffered bytes of an output stream.
#include <cstddef>
#include <cstdint>

struct D2CrtFile { uint32_t ptr, cnt, base, flag; int32_t file; };  // 32-bit retail FILE head
static_assert(offsetof(D2CrtFile, flag) == 0xC && offsetof(D2CrtFile, file) == 0x10, "FILE");

// write(fd, base, n) is __write (0x481F); base is a retail 32-bit address, passed through as-is.
// 1.13c D2Common+0x0286b
extern "C" int32_t d2common_flush_286b(D2CrtFile* f, int32_t (*write)(int32_t, uint32_t, uint32_t))
{
	int32_t r = 0;
	if ((f->flag & 3) == 2 && (f->flag & 0x108) != 0) {
		const uint32_t n = f->ptr - f->base;
		if (static_cast<int32_t>(n) > 0) {
			if (static_cast<uint32_t>(write(f->file, f->base, n)) == n) {
				if (static_cast<int8_t>(f->flag) < 0) f->flag &= ~2u;
			} else { f->flag |= 0x20; r = -1; }
		}
	}
	f->cnt = 0;
	f->ptr = f->base;
	return r;
}
