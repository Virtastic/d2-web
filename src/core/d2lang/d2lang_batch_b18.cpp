// D2Lang 0x8740 Unicode::Personalize
#include <cstdint>
typedef uint16_t u16;
static int wl(const u16* s) { int n = 0; if (s) while (s[n]) n++; return n; }
static u16* app(u16* d, const char* a) { while (*d) d++; while (*a) *d++ = (uint8_t)*a++; *d = 0; return d; }
static void appw(u16* d, const u16* s) { while (*d) d++; while ((*d++ = *s++)) {} }

extern "C" bool d2lang_8520(const char* set, const uint16_t* wide);

// 1.13c D2Lang+0x8740: dst = name(p2) + title(p3) joined per language, cap = capacity in units.
extern "C" void d2lang_8740(u16* dst, const u16* p2, const u16* p3, int cap, int lang)
{
	int l2 = wl(p2), l3 = wl(p3);
	if (cap < l3 + 5 + l2) {
		if (cap <= l3) return;
		const u16* s = p3; u16* d = dst;
		do *d++ = *s; while (*s++);
		return;
	}
	const char* suf; bool first2; // first2: p2 leads, else p3 leads
	switch (lang) {
	case 0: case 12: suf = d2lang_8520("s", p2) ? "' " : "'s "; first2 = true; break;
	case 1: suf = " de "; first2 = false; break;
	case 2: suf = d2lang_8520("sz", p2) ? "' " : "s "; first2 = true; break;
	case 3: suf = d2lang_8520("aeiou", p2) ? " d'" : " de "; first2 = false; break;
	case 5: suf = " di "; first2 = false; break;
	case 10: dst[0] = 0; appw(dst, p3); app(dst, "("); appw(dst, p2); app(dst, ") "); return;
	default: dst[0] = 0; app(dst, "("); appw(dst, p2); app(dst, ") "); appw(dst, p3); return;
	}
	dst[0] = 0;
	appw(dst, first2 ? p2 : p3); app(dst, suf); appw(dst, first2 ? p3 : p2);
}
