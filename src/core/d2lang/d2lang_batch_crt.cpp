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
