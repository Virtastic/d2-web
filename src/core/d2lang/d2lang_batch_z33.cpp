// 1.13c D2Lang implementation batch z33: Unicode string helpers (UTF-16 code units, uint16_t).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

typedef uint16_t z33_wchar;

// Seams: retail globals and callees not yet mapped.
extern "C" const uint16_t z33_toUpperTable[256];            // D2Lang _toUpperTable
extern "C" int32_t z33_utf8_len_of_unit(uint16_t nUnit);   // D2Lang+0x8DC0 (-1 on failure)
extern "C" int32_t z33_utf8_lead_len(int32_t nRemain);     // D2Lang+0x8BB0 (as called: arg = remaining count)
extern "C" void z33_toUnicode(z33_wchar* pDst, const char* pSrc, int32_t nMax); // Unicode::toUnicode
extern "C" z33_wchar* z33_wstrstr(const z33_wchar* pHay, const z33_wchar* pNeedle); // Unicode::strstr
extern "C" const z33_wchar* z33_ptr(uint32_t nLink);        // 32-bit retail pointer -> host pointer
extern "C" [[noreturn]] void z33_fatal(uint32_t nStringId); // Ordinal_10265 + Ordinal_10024 + _exit(-1)
extern "C" void z33_free(uint32_t nPtr);                    // Ordinal_403(ptr,"delete[]",-1,0)
extern "C" uint32_t z33_g_sysMapPtr;  // DAT_6fc10a50
extern "C" uint32_t z33_g_rangeCount; // DAT_6fc10a54 (low byte used)
extern "C" const uint8_t* z33_g_ranges; // DAT_6fc10a58 (byte pairs lo,hi); also freed via link
extern "C" uint32_t z33_g_ranges_link;  // DAT_6fc10a58 as 32-bit link
extern "C" uint32_t z33_g_dbcsMap;    // DAT_6fc10a5c

static z33_wchar z33_up(z33_wchar c) { return c < 0x100 ? z33_toUpperTable[c] : c; }

// 1.13c D2Lang+0x0B0B0 (#10040)
extern "C" int32_t z33_strlen(const z33_wchar* p)
{
	int32_t n = 0;
	if (!p) return 0;
	while (p[n]) ++n;
	return n;
}

// 1.13c D2Lang+0x0B170 (#10035)
extern "C" z33_wchar* z33_strchr(const z33_wchar* p, z33_wchar c)
{
	for (; *p; ++p)
		if (*p == c) return const_cast<z33_wchar*>(p);
	return nullptr;
}

// 1.13c D2Lang+0x0B200 (#10036)
extern "C" int32_t z33_strcmp(const z33_wchar* a, const z33_wchar* b)
{
	while (*a && *b && *a == *b) { ++a; ++b; }
	return *a < *b ? -1 : (*b < *a);
}

// 1.13c D2Lang+0x0B610 (#10039)
extern "C" int32_t z33_stricmp(const z33_wchar* a, const z33_wchar* b)
{
	for (;;)
	{
		if (!*a) return *b ? -1 : 0;
		if (!*b) return 1;
		z33_wchar x = z33_up(*a), y = z33_up(*b);
		if (x != y) return x < y ? -1 : 1;
		++a; ++b;
	}
}

// 1.13c D2Lang+0x0B120 (#10041)
extern "C" z33_wchar* z33_strncat(z33_wchar* d, const z33_wchar* s, int32_t n)
{
	z33_wchar* r = d;
	while (*d) ++d;
	while (n != 0 && *s) { --n; *d++ = *s++; }
	*d = 0;
	return r;
}

// 1.13c D2Lang+0x0B1B0 (#10042)
extern "C" int32_t z33_strncmp(const z33_wchar* a, const z33_wchar* b, uint32_t n)
{
	while (*a && *b && n)
	{
		z33_wchar x = *a, y = *b;
		++a; ++b;
		if (x != y) return x < y ? -1 : 1;
		--n;
	}
	return 0;
}

// 1.13c D2Lang+0x0B560 (#10045)
extern "C" int32_t z33_strnicmp(const z33_wchar* a, const z33_wchar* b, uint32_t n)
{
	z33_wchar c = *a;
	z33_wchar x = z33_up(c), y = z33_up(*b);
	while (c && *b && n && x == y)
	{
		x = z33_up(*a++); y = z33_up(*b++);
		c = *a; --n;
	}
	return y <= x ? (y < x) : -1;
}

// 1.13c D2Lang+0x0B250 (#10033): args = the caller's variadic stack words (ints or 32-bit links).
extern "C" void z33_sprintf(int32_t nMax, z33_wchar* d, const z33_wchar* f, const uint32_t* args)
{
	if (!f) return;
	*d = 0;
	int32_t n = 0;
	const z33_wchar pct[2] = { 0x25, 0 };
	while (*f)
	{
		z33_wchar kind = 0;
		const z33_wchar* hit = z33_wstrstr(f, pct);
		if (hit) kind = hit[1];
		for (; f != hit && *f; ++f)
		{
			if (nMax <= n) { d[-1] = 0; return; }
			*d++ = *f; ++n;
		}
		if (nMax <= n) { d[-1] = 0; return; }
		if (!*f) { *d = 0; return; }
		if (!hit) z33_fatal(0x10d);
		uint32_t v = *args;
		switch (kind)
		{
		case 0: d[0] = 0x25; d[1] = 0; return;
		default: z33_fatal(0x154);
		case 0x25:
			if (nMax <= ++n) return;
			d[0] = 0x25; d[1] = 0; ++d;
			break;
		case 'd': case 'u':
		{
			char s[16]; z33_wchar w[16] = {0};
			snprintf(s, sizeof s, kind == 'd' ? "%d" : "%u", kind == 'd' ? (int)(int32_t)v : (int)v);
			if (kind == 'u') snprintf(s, sizeof s, "%u", v);
			z33_toUnicode(w, s, 15);
			int32_t l = z33_strlen(w);
			if (nMax <= l + 1 + n) return;
			memcpy(d, w, (l + 1) * 2);
			n += l; d += l;
			break;
		}
		case 's':
		{
			const z33_wchar* s = z33_ptr(v);
			int32_t l = s ? z33_strlen(s) : 0;
			if (!l || nMax <= l + 1 + n)
			{
				// strncat(d, s, nMax - n - 1) (retail passes the possibly-null string through)
				z33_strncat(d, s, nMax - n - 1);
				return;
			}
			memcpy(d, s, (l + 1) * 2);
			d += l; n += l;
		}
		}
		f += 2; ++args;
	}
}

// 1.13c D2Lang+0x08E80 (#10053)
extern "C" char* z33_toUtf(char* d, const z33_wchar* s, int32_t nMax)
{
	int32_t n = 0;
	while (*s && n < nMax - 1)
	{
		int32_t l = z33_utf8_len_of_unit(*s);
		if (l == -1) break;
		++s; n += l;
	}
	d[n] = 0;
	return d;
}

// 1.13c D2Lang+0x0ACF0 (#10056)
extern "C" uint32_t z33_unicodeWidth(const char* p, int32_t nMax)
{
	if (z33_g_dbcsMap == 0) return (uint32_t)strlen(p);
	uint32_t w = 0;
	int32_t i = 0;
	if (0 < nMax)
	{
		do
		{
			uint8_t b = (uint8_t)p[i];
			if (!b) return w;
			if (b < 0x20) ++i;
			else
			{
				int32_t k = 0;
				bool single = false;
				for (; z33_g_rangeCount && k < (int32_t)(uint8_t)z33_g_rangeCount; ++k)
					if (z33_g_ranges[k * 2] <= b && b <= z33_g_ranges[k * 2 + 1]) { single = true; break; }
				i += single ? 1 : 2;
			}
			++w;
		} while (i < nMax);
	}
	return w;
}

// 1.13c D2Lang+0x08D70 (#10057)
extern "C" uint32_t z33_unicodenwidth(const char* p, int32_t nMax)
{
	uint32_t w = 0;
	if (0 < nMax)
	{
		while (*p)
		{
			int32_t l = z33_utf8_lead_len(nMax);
			if (l == -1) { ++p; l = -1; }
			else
			{
				for (int32_t k = 0; k < l || (k == 0 && l <= 0 && false); ++k)
					if (!*p++) return w;
				++w; l = -l;
			}
			nMax += l;
			if (nMax < 1) return w;
		}
	}
	return w;
}

// 1.13c D2Lang+0x0AC40 (#10058)
extern "C" void z33_unloadSysMap()
{
	if (z33_g_ranges_link) { z33_free(z33_g_ranges_link); z33_g_ranges_link = 0; }
	if (z33_g_sysMapPtr) { z33_free(z33_g_sysMapPtr); z33_g_sysMapPtr = 0; }
	if (z33_g_dbcsMap) { z33_free(z33_g_dbcsMap); z33_g_dbcsMap = 0; }
}
