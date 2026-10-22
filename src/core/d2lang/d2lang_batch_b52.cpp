// 1.13c D2Lang implementation batch: statically linked MSVC 2003 CRT heap/string routines.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// 1.13c D2Lang+0x04740: _strlen -- length up to the first NUL.
extern "C" size_t d2lang_crt_strlen(const char* s) { return strlen(s); }

// 1.13c D2Lang+0x043DF: _malloc -- __nh_malloc(size, new-handler mode); host malloc.
extern "C" void* d2lang_crt_malloc(size_t n) { return malloc(n); }

// 1.13c D2Lang+0x03091: _free -- NULL is a no-op.
extern "C" void d2lang_crt_free(void* p) { free(p); }

// 1.13c D2Lang+0x03102: _calloc -- fails when a*b would exceed 0xFFFFFFE0 bytes; zero-filled;
// a zero-byte request allocates one byte.
extern "C" void* d2lang_crt_calloc(uint32_t a, uint32_t b)
{
	if (a != 0 && b > 0xFFFFFFE0u / a)
		return nullptr;
	uint32_t n = a * b;
	return calloc(n ? n : 1, 1);
}

// MSVC 2003 FILE layout (32-bit).
struct D2LangMsvcFile
{
	uint32_t ptr, cnt, base;
	int32_t flag, file, charbuf, bufsiz;
	uint32_t tmpfname;
};

// 1.13c D2Lang+0x05D31: __getbuf -- give the stream a 4096-byte buffer, else the 2-byte charbuf.
// Returns the buffer (the 32-bit FILE holds guest pointers; ours takes a host buffer).
// ponytail: mallocated base is a host pointer, narrowed only where FILE is wasm32-addressable.
extern "C" void* d2lang_crt_getbuf(D2LangMsvcFile* f, uint32_t* nmallocs, uint32_t charbufAddr)
{
	++*nmallocs;
	void* b = malloc(0x1000);
	if (!b)
	{
		f->flag |= 4;
		f->base = charbufAddr;
		f->bufsiz = 2;
	}
	else
	{
		f->flag |= 8;
		f->bufsiz = 0x1000;
		f->base = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(b));
	}
	f->cnt = 0;
	f->ptr = f->base;
	return b;
}
