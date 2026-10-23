// 1.13c D2Lang+0x55D5: CRT __crtLCMapStringA. Win32 and retail globals via env.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct d2lang_lcmap_env
{
	int (*lcMapW)(uint32_t lcid, uint32_t fl, const uint16_t* s, int n, uint16_t* d, int dn);
	int (*lcMapA)(uint32_t lcid, uint32_t fl, const char* s, int n, char* d, int dn);
	int (*mbToWc)(uint32_t cp, uint32_t fl, const char* s, int n, uint16_t* d, int dn);
	int (*wcToMb)(uint32_t cp, uint32_t fl, const uint16_t* s, int n, char* d, int dn, const char* def, int* used);
	uint32_t (*getLastError)();
	long (*lcidToCp)(uint32_t lcid);                                                       // 0x6BBE
	char* (*cpConvert)(uint32_t from, uint32_t to, const char* s, int* n, char* out, int outn); // 0x6C05
	int state;       // DAT_6fc10928
	uint32_t lcid;   // DAT_6fc10894
	uint32_t acp;    // DAT_6fc108a4
};
static d2lang_lcmap_env d2lang_lcmap_env_g;
extern "C" d2lang_lcmap_env* d2lang_lcmap_getEnv() { return &d2lang_lcmap_env_g; }

// 1.13c D2Lang+0x55D5
extern "C" uint32_t d2lang_lcmap55d5_sub_55D5(uint32_t lcid, uint32_t flags, const char* src, int cch,
                                              char* dst, int dcch, uint32_t cp, int bErr)
{
	d2lang_lcmap_env& e = d2lang_lcmap_env_g;
	static const uint16_t kEmpty[1] = {0};
	if (e.state == 0)
	{
		if (e.lcMapW(0, 0x100, kEmpty, 1, nullptr, 0) == 0)
		{
			if (e.getLastError() == 0x78) e.state = 2;
		}
		else e.state = 1;
	}
	if (cch > 0) { int n = 0; while (n < cch && src[n]) n++; cch = n < cch ? n : cch; }
	if (e.state != 2 && e.state != 0)
	{
		if (e.state != 1) return 0;
		if (cp == 0) cp = e.acp;
		int wn = e.mbToWc(cp, bErr ? 9 : 1, src, cch, nullptr, 0);
		if (wn == 0) return 0;
		uint16_t* w = (uint16_t*)malloc((size_t)wn * 2);
		if (!w) return 0;
		uint32_t r = 0;
		if (e.mbToWc(cp, 1, src, cch, w, wn) != 0)
		{
			r = (uint32_t)e.lcMapW(lcid, flags, w, wn, nullptr, 0);
			if (r != 0)
			{
				if (!(flags & 0x400))
				{
					uint16_t* m = (uint16_t*)malloc((size_t)r * 2);
					if (m)
					{
						if (e.lcMapW(lcid, flags, w, wn, m, (int)r) != 0)
							r = (uint32_t)e.wcToMb(cp, 0, m, (int)r, dst, dst ? dcch : 0, nullptr, nullptr);
						free(m);
					}
				}
				else if (dcch != 0 && (int)r <= dcch)
					e.lcMapW(lcid, flags, w, wn, (uint16_t*)dst, dcch);
			}
		}
		free(w);
		return r;
	}
	if (lcid == 0) lcid = e.lcid;
	if (cp == 0) cp = e.acp;
	long lcp = e.lcidToCp(lcid);
	if (lcp == -1) return 0;
	uint32_t lc = (uint32_t)lcp;
	if (lc == cp) return (uint32_t)e.lcMapA(lcid, flags, src, cch, dst, dcch);
	char* conv = e.cpConvert(cp, lc, src, &cch, nullptr, 0);
	if (!conv) return 0;
	uint32_t r = 0;
	char* back = nullptr;
	int n = e.lcMapA(lcid, flags, conv, cch, nullptr, 0);
	if (n != 0)
	{
		char* tmp = (char*)calloc((size_t)n, 1);
		if (tmp)
		{
			n = e.lcMapA(lcid, flags, conv, cch, tmp, n);
			if (n != 0)
			{
				back = e.cpConvert(lc, cp, tmp, &n, dst, dcch);
				r = back ? (uint32_t)n : 0;
			}
			free(tmp);
		}
	}
	free(conv);
	if (back && back != dst) free(back);
	return r;
}
