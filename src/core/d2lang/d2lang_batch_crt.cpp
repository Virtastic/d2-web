// 1.13c D2Lang implementation batch: statically linked MSVC 2003 CRT routines.
// Retail's locale branch of _stricmp is the C locale here (no setlocale in the DLL).
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// 1.13c D2Lang+0x01540: _exit(code); terminates without atexit handlers (doexit(code,0,0)).
extern "C" void d2lang_crt_exit(int code) { _Exit(code); }

// 1.13c D2Lang+0x015C0: _strncpy -- copy up to n bytes, zero-pad the rest after the NUL.
extern "C" char* d2lang_crt_strncpy(char* dst, const char* src, size_t n)
{
	size_t i = 0;
	for (; i < n && src[i] != '\0'; ++i)
		dst[i] = src[i];
	for (; i < n; ++i)
		dst[i] = '\0';
	return dst;
}

// 1.13c D2Lang+0x0182F: __stricmp -- ASCII case-fold compare, returns lower(a)-lower(b).
extern "C" int d2lang_crt_stricmp(const char* a, const char* b)
{
	int ca, cb;
	do
	{
		ca = static_cast<unsigned char>(*a++);
		cb = static_cast<unsigned char>(*b++);
		if (ca >= 'A' && ca <= 'Z') ca += 32;
		if (cb >= 'A' && cb <= 'Z') cb += 32;
	} while (ca != 0 && ca == cb);
	return ca - cb;
}

// 1.13c D2Lang+0x01898: _sprintf -- unbounded formatted write, returns chars written.
extern "C" int d2lang_crt_sprintf(char* dst, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int n = vsprintf(dst, fmt, ap);
	va_end(ap);
	return n;
}

// 1.13c D2Lang+0x7C00: fatal-read-check tail (register args: ESI=handle, EDI=expected, local=actual size).
// handle==0 -> msg 0x87; Fog#10104 read fails -> Storm#276 then msg 0x8f; size mismatch -> msg 0x92;
// otherwise returns. Failure: Fog#10265(msg) -> Fog#10024(fmt 0x6fc0d274, text), _exit(-1).
struct D2LangReadCheckHooks {
	int (*fogRead)();            // Fog#10104
	void (*stormLastError)();    // Storm#276
	unsigned (*fogMsg)(unsigned); // Fog#10265
	void (*fogLog)(unsigned);    // Fog#10024 with the 0x6fc0d274 format
};
extern "C" void d2lang_readcheck_7c00(const void* handle, int expected, int actual, const D2LangReadCheckHooks& h)
{
	unsigned msg;
	if (!handle) msg = 0x87;
	else if (!h.fogRead()) { h.stormLastError(); msg = 0x8f; }
	else if (expected == actual) return;
	else msg = 0x92;
	h.fogLog(h.fogMsg(msg));
	d2lang_crt_exit(-1);
}

// 1.13c D2Lang+0x03E37: ___free_lc_time -- free the 43 owned strings of a time-locale block.
extern "C" void d2lang_crt_free_lc_time(void** p)
{
	if (!p) return;
	for (int i = 0; i < 43; i++) free(p[i]);
}
