// 1.13c D2Lang+0x05ACA: MSVC CRT _write_nolock(fd, buf, cnt). Seams replace the Win32 calls
// and the module's ioinfo table (entry 0x24 bytes: +0 handle, +4 osfile flags).
#include <cstdint>
extern "C" int* d2lang_crt_errno_ptr();
extern "C" unsigned long* d2lang_crt_doserrno_ptr();

struct D2LangIoInfo { void* handle; uint8_t osfile; uint8_t pad[0x1f]; };
struct D2LangWriteSeams {
	D2LangIoInfo** table;                                              // 32 entries per block
	int64_t (*lseek)(int fd, int64_t off, int whence);                 // __lseeki64_lk
	int (*write_file)(void* h, const void* b, uint32_t n, uint32_t* w); // WriteFile, nonzero = ok
	unsigned long (*last_error)();                                     // GetLastError
	void (*dosmaperr)(unsigned long);                                  // __dosmaperr
};

extern "C" int d2lang_crt_write_nolock(const D2LangWriteSeams* s, unsigned fd, const char* buf, unsigned cnt)
{
	unsigned wrote = 0; int lf = 0; unsigned long err = 0; uint32_t w = 0;
	if (!cnt) return 0;
	D2LangIoInfo* io = &s->table[fd >> 5][fd & 0x1f];
	if (io->osfile & 0x20) s->lseek(fd, 0, 2);
	if (!(io->osfile & 0x80)) {
		if (!s->write_file(io->handle, buf, cnt, &w)) err = s->last_error(); else wrote = w;
	} else {
		const char* p = buf;
		do {
			char tmp[100 * 0 + 0x400 + 2]; int n = 0; // ponytail: 0x400+2 covers the CRLF overshoot
			while ((unsigned)(p - buf) < cnt && n < 0x400) {
				char c = *p++;
				if (c == '\n') { ++lf; tmp[n++] = '\r'; }
				tmp[n++] = c;
			}
			if (!s->write_file(io->handle, tmp, n, &w)) { err = s->last_error(); break; }
			wrote += w;
			if (n > (int)w || (unsigned)(p - buf) >= cnt) break;
		} while (true);
	}
	if (wrote) return (int)(wrote - lf);
	if (err) {
		if (err == 5) { *d2lang_crt_errno_ptr() = 9; *d2lang_crt_doserrno_ptr() = 5; return -1; }
		s->dosmaperr(err); return -1;
	}
	if ((io->osfile & 0x40) && *buf == 0x1a) return 0;
	*d2lang_crt_errno_ptr() = 0x1c; *d2lang_crt_doserrno_ptr() = 0;
	return -1;
}
