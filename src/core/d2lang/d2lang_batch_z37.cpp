// 1.13c D2Lang implementation batch z37: record lookup helpers and MSVC CRT leaves.
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Seams: retail callees and globals not yet mapped.
extern "C" uint8_t* z37_ptr(uint32_t nLink);                        // 32-bit retail pointer -> host pointer
extern "C" const char* z37_string(uint32_t nId);                    // Ordinal_10265
extern "C" void z37_report(const char* pFmt, const char* pStr);     // Ordinal_10024(&DAT_6fc0d274, str)
extern "C" int32_t z37_mbToWide(uint16_t* pDst, const char* pSrc, int32_t nBytes, int32_t nMax); // Ordinal_904 (D2Lang+0x7AD0 path); nonzero = fail
extern "C" uint32_t z37_hash(const char* pKey, uint32_t nBuckets);  // D2Lang+0x8FE0
extern "C" void z37_crc(uint16_t* pCrc, const uint8_t* pBase, uint32_t nA, uint32_t nB); // D2Lang+0x92F0
extern "C" void z37_loadTbl(uint32_t nPath, int32_t* pOutSize);     // Ordinal_10102/10105/10042/10103 chain (D2Lang+0x7EF0 body)
extern "C" uint32_t z37_g_tblPath;   // DAT_6fc10a8c
extern "C" uint32_t z37_g_ready;     // DAT_6fc10a64
extern "C" uint32_t z37_g_tbl;       // DAT_6fc10a68
extern "C" int16_t z37_g_key[4];     // DAT_6fc10558..64

// 1.13c D2Lang+0x01540
extern "C" [[noreturn]] void z37_exit(int32_t nCode) { exit(nCode); }

static void z37_die(uint32_t nId) { z37_report("%s", z37_string(nId)); z37_exit(-1); }

// 1.13c D2Lang+0x09050 (base arrives in EAX; guessed return: slot value of the id's node)
extern "C" uint32_t z37_nodeSlot(uint8_t* pBase, uint16_t nId, uint32_t* pSlots)
{
	uint32_t i = nId;
	if (*(uint16_t*)(pBase + 2) <= nId) i = 500;
	i = *(uint16_t*)(pBase + 0x15 + i * 2);
	uint32_t e;
	if ((int32_t)i >= *(int32_t*)(pBase + 4)) e = 0x104;
	else {
		const char* n = (const char*)(pBase + 0x15 + *(uint16_t*)(pBase + 2) * 2 + i * 0x11);
		if (n >= (const char*)(pBase + *(uint32_t*)(pBase + 0x11))) e = 0x108;
		else if (*n != 1) e = 0x109;
		else if (pSlots[i]) return pSlots[i];
		else e = 0x10b;
	}
	z37_die(e);
	return 0;
}

// 1.13c D2Lang+0x07EF0
extern "C" uint8_t* z37_openTbl(uint32_t nPath, int32_t* pOut)
{
	// ponytail: body is an Ordinal chain (open/size/alloc/read/close) behind one seam; error ids 0x59/0x5f/0x49 inside it.
	int32_t sz = 0;
	z37_loadTbl(nPath, &sz);
	if (!sz) return nullptr;
	if (pOut) *pOut = sz;
	return z37_ptr(nPath);
}

// 1.13c D2Lang+0x0122E (__ultoa)
extern "C" char* z37_ultoa(unsigned long v, char* d, int r)
{
	char t[34]; int n = 0;
	do { int x = v % r; t[n++] = x < 10 ? '0' + x : 'a' + x - 10; v /= r; } while (v);
	for (int i = 0; i < n; ++i) d[i] = t[n - 1 - i];
	d[n] = 0;
	return d;
}

// 1.13c D2Lang+0x012E6 (__ui64toa)
extern "C" char* z37_ui64toa(uint64_t v, char* d, int r)
{
	char t[66]; int n = 0;
	do { int x = v % r; t[n++] = x < 10 ? '0' + x : 'a' + x - 10; v /= r; } while (v);
	for (int i = 0; i < n; ++i) d[i] = t[n - 1 - i];
	d[n] = 0;
	return d;
}

// 1.13c D2Lang+0x015C0 (_strncpy: copy, zero-pad to count)
extern "C" char* z37_strncpy(char* d, const char* s, size_t n)
{
	size_t i = 0;
	for (; i < n && s[i]; ++i) d[i] = s[i];
	for (; i < n; ++i) d[i] = 0;
	return d;
}

// 1.13c D2Lang+0x0182F (__stricmp, "C" locale branch)
extern "C" int z37_stricmp(const char* a, const char* b)
{
	int x, y;
	do {
		x = (uint8_t)*a++; y = (uint8_t)*b++;
		if (x >= 'A' && x <= 'Z') x += 32;
		if (y >= 'A' && y <= 'Z') y += 32;
	} while (x && x == y);
	return x - y;
}

// 1.13c D2Lang+0x01898 (_sprintf)
extern "C" int z37_sprintf(char* d, const char* f, ...)
{
	va_list a; va_start(a, f);
	int n = vsnprintf(d, 0x7fffffff, f, a);
	va_end(a);
	return n;
}

// 1.13c D2Lang+0x08370 (Unicode::utf8ToUnicode; guessed from a messy decompile: convert strlen+1 bytes, copy up to count)
extern "C" uint16_t* z37_utf8ToUnicode(uint16_t* pDst, const char* pSrc, int32_t nMax)
{
	uint16_t buf[0x400];
	int32_t len = (int32_t)strlen(pSrc) + 1;
	if (z37_mbToWide(buf, pSrc, len, 0x3ff)) len = 0;
	buf[len] = 0;
	int32_t i = 0;
	while (i < nMax && buf[i]) { pDst[i] = buf[i]; ++i; }
	if (buf[i] == 0) pDst[i] = 0; else pDst[i - 1] = 0;
	return pDst;
}

// 1.13c D2Lang+0x094E0
extern "C" uint16_t* z37_loadValidated(void)
{
	uint8_t* p = z37_openTbl(z37_g_tblPath, nullptr);
	uint16_t crc = 0;
	z37_crc(&crc, p, *(uint32_t*)(p + 9), *(uint32_t*)(p + 0x11));
	if (crc != *(uint16_t*)p) z37_die(0xd0);
	uint32_t sz = *(uint32_t*)(p + 0x11);
	uint8_t* q = p + *(uint16_t*)(p + 2) * 2 + 0x20;
	for (int32_t i = 0; i < *(int32_t*)(p + 4); ++i, q += 0x11) {
		if (sz <= *(uint32_t*)(q - 4)) z37_die(0xe2);
		if (sz <= *(uint32_t*)q) z37_die(0xe3);
	}
	return (uint16_t*)p;
}

// 1.13c D2Lang+0x09590
extern "C" void z37_loadKeys(void)
{
	static const uint16_t ids[4] = {0x14d0, 0x14d1, 0x14d2, 0x14d3};
	static const int16_t def[4] = {0x4b, 0x4d, 0x42, 0x54};
	for (int k = 0; k < 4; ++k) {
		if (!z37_g_ready) z37_die(0x248);
		const uint16_t* w = (const uint16_t*)z37_ptr(z37_nodeSlot(z37_ptr(z37_g_tbl), ids[k], (uint32_t*)z37_ptr(z37_g_tbl)));
		char s[8]; int n = 0;
		while (n < 4 && w[n]) { s[n] = (char)w[n]; ++n; }
		s[n] = 0;
		z37_g_key[k] = n < 2 ? (int16_t)s[0] : def[k];
	}
}

// 1.13c D2Lang+0x09360 (returns hi:lo; lo = node u16 at +1, 0xffff if not found; *pOut = slot value)
extern "C" uint32_t z37_findByName(uint8_t* pBase, uint32_t* pSlots, const uint8_t* pKey, int32_t* pOut)
{
	uint8_t* nodes = pBase + 0x15 + *(uint16_t*)(pBase + 2) * 2;
	uint32_t nBuckets = *(uint32_t*)(pBase + 4), nLeft = *(uint32_t*)(pBase + 0xd), tried = 0;
	uint32_t i = z37_hash((const char*)pKey, nBuckets);
	while (tried < nLeft) {
		uint8_t* n = nodes + i * 0x11;
		++tried;
		if (*n == 1) {
			const uint16_t* a = (const uint16_t*)(pBase + *(uint32_t*)(n + 7));
			const uint16_t* b = (const uint16_t*)pKey;
			while (*a && *a == *b) { ++a; ++b; }
			if (*a == *b) {
				if (!pSlots[i]) z37_die(0x298);
				*pOut = (int32_t)pSlots[i];
				return *(uint16_t*)(n + 1);
			}
		}
		i = (i + 1) % nBuckets;
	}
	return 0xffff;
}
