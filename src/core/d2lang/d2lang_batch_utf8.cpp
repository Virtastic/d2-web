// 1.13c D2Lang implementation batch: UTF-8 helpers (+0x08BB0 +0x08C40 +0x08DC0) and strncmp (+0x01580).
// Retail passes the byte pointer / length in registers; ours takes them as arguments.
#include <cstdint>
#include <cstddef>

namespace
{
// D2Lang.[0x6FC102E8..): 16-byte stride rows, zero cmask terminates.
struct Utf8Row { uint16_t cmask, cval, shift, pad; int32_t last, first; };
const Utf8Row gUtf8[7] = {
	{ 0x80, 0x00, 0, 0, 0x7F, 0 },
	{ 0xE0, 0xC0, 6, 0, 0x7FF, 0x80 },
	{ 0xF0, 0xE0, 12, 0, 0xFFFF, 0x800 },
	{ 0xF8, 0xF0, 18, 0, 0x1FFFFF, 0x10000 },
	{ 0xFC, 0xF8, 24, 0, 0x3FFFFFF, 0x200000 },
	{ 0xFE, 0xFC, 30, 0, 0x7FFFFFFF, 0x4000000 },
	{ 0, 0, 0, 0, 0, 0 },
};
}

// 1.13c D2Lang+0x01580: MSVC _strncmp.
extern "C" int d2lang_strncmp(const char* a, const char* b, size_t n)
{
	for (; n; --n, ++a, ++b)
	{
		if (*a != *b)
			return (uint8_t)*b > (uint8_t)*a ? -1 : 1;
		if (!*a)
			break;
	}
	return 0;
}

// 1.13c D2Lang+0x08BB0: validate one UTF-8 sequence in nLen bytes; returns its length or -1.
extern "C" int d2lang_utf8_check(const uint8_t* p, int nLen)
{
	if (nLen < 1)
		return -1;
	const Utf8Row* r = gUtf8;
	uint32_t v = *p;
	int n = 0;
	if ((int16_t)r->cmask == 0)
		return -1;
	while (++n, ((int16_t)r->cmask & *p) != (int16_t)r->cval)
	{
		if (nLen <= n)
			return -1;
		++p;
		if (((*p ^ 0x80) & 0xc0) != 0)
			return -1;
		++r;
		v = v << 6 | (*p ^ 0x80);
		if (r->cmask == 0)
			return -1;
	}
	return (int32_t)(r->last & v) >= r->first ? n : -1;
}

// 1.13c D2Lang+0x08C40: decode one UTF-8 sequence to a UCS-2 unit; returns its length or -1.
extern "C" int d2lang_utf8_decode(const uint8_t* p, int nLen, uint16_t* pOut)
{
	if (nLen < 1)
		return -1;
	const Utf8Row* r = gUtf8;
	uint8_t b = *p;
	uint32_t v = (uint32_t)(int32_t)(int16_t)b;
	int n = 0;
	for (;;)
	{
		if (r->cmask == 0)
			return -1;
		++n;
		if ((uint16_t)(r->cmask & b) == r->cval)
		{
			if ((int32_t)(v & (uint32_t)r->last) < r->first)
				return -1;
			*pOut = (uint16_t)(v & (uint32_t)r->last);
			return n;
		}
		if (nLen <= n)
			return -1;
		uint8_t t = *++p ^ 0x80;
		if ((t & 0xc0) != 0)
			return -1;
		++r;
		v = v << 6 | (uint32_t)(int32_t)(int16_t)t;
		b = *p;
	}
}

// 1.13c D2Lang+0x08DC0: encode one UCS-2 unit as UTF-8 into pOut; returns the length or -1.
extern "C" int d2lang_utf8_encode(uint16_t wc, uint8_t* pOut)
{
	const Utf8Row* r = gUtf8;
	int n = 0;
	while (r->cmask != 0)
	{
		++n;
		if ((int32_t)wc <= r->last)
		{
			int s = r->shift;
			*pOut = (uint8_t)((int32_t)wc >> (s & 0x1f)) | (uint8_t)r->cval;
			while (s > 0)
			{
				s -= 6;
				*++pOut = ((uint8_t)((int32_t)wc >> (s & 0x1f))) & 0x3f | 0x80;
			}
			return n;
		}
		++r;
	}
	return -1;
}
