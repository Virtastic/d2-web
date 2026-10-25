// 1.13c D2Lang implementation batch: +0x011C6..+0x013FB (statically linked MSVC CRT pieces).
// Not ported: +0x013B9 (___crtExitProcess, mscoree/ExitProcess), +0x013E9/+0x013F2 (CRT lock 8
// enter/leave; callees 0x22CA/0x2215 have no port).
#include <cctype>
#include <cstdint>

// Digit loop shared by xtoa/x64toa: digits 0-9 then a-z, written reversed then flipped in place.
static void d2lang_digits(uint64_t val, char* buf, uint32_t radix)
{
	char* p = buf;
	do
	{
		uint32_t d = static_cast<uint32_t>(val % radix);
		val /= radix;
		*p++ = static_cast<char>(d < 10 ? d + '0' : d + 'W');
	} while (val != 0);
	*p = '\0';
	for (char* last = p - 1; buf < last; ++buf, --last)
	{
		char t = *last;
		*last = *buf;
		*buf = t;
	}
}

// 1.13c D2Lang+0x011C6 xtoa: optional '-' (value negated as 32-bit), then digits.
extern "C" void d2_fn_d2lang_11c6(uint32_t val, char* buf, uint32_t radix, int neg)
{
	if (neg)
	{
		*buf++ = '-';
		val = 0u - val;
	}
	d2lang_digits(val, buf, radix);
}

// 1.13c D2Lang+0x01204 _itoa: negative sign only for radix 10 and value < 0.
extern "C" char* d2lang_1204(int32_t value, char* dest, int32_t radix)
{
	d2_fn_d2lang_11c6(static_cast<uint32_t>(value), dest, static_cast<uint32_t>(radix), radix == 10 && value < 0);
	return dest;
}

// 1.13c D2Lang+0x0122E _ultoa
extern "C" char* d2_fn_d2lang_122e(uint32_t value, char* dest, int32_t radix)
{
	d2_fn_d2lang_11c6(value, dest, static_cast<uint32_t>(radix), 0);
	return dest;
}

// 1.13c D2Lang+0x01248 x64toa: 64-bit variant (negation over the full 64 bits).
extern "C" void d2_fn_d2lang_1248(uint64_t val, char* buf, uint32_t radix, int neg)
{
	if (neg)
	{
		*buf++ = '-';
		val = 0 - val;
	}
	d2lang_digits(val, buf, radix);
}

// 1.13c D2Lang+0x012E6 _ui64toa
extern "C" char* d2_fn_d2lang_12e6(uint64_t val, char* dest, int32_t radix)
{
	d2_fn_d2lang_1248(val, dest, static_cast<uint32_t>(radix), 0);
	return dest;
}

// 1.13c D2Lang+0x01301 _isalpha (retail mask 0x103 = upper|lower|alpha); C-locale table.
extern "C" int d2_fn_d2lang_1301(int c) { return isalpha(c); }

// 1.13c D2Lang+0x01340 _isspace (mask 0x08)
extern "C" int d2_fn_d2lang_1340(int c) { return isspace(c); }

// 1.13c D2Lang+0x0137A _isalnum (mask 0x107)
extern "C" int d2_fn_d2lang_137a(int c) { return isalnum(c); }

// 1.13c D2Lang+0x013FB __initterm: call each non-null function pointer in [first, last).
extern "C" void d2_fn_d2lang_13fb(void (**first)(), void (**last)())
{
	for (; first < last; ++first)
		if (*first)
			(*first)();
}
