// 1.13c D2Lang+0x4BB1: CRT _setmbcp(cp). The module globals it rewrites are one block (d2lang_mbcp_state, laid out as
// retail's 0x11AC4..0x11BFC); the builtin code-page table (0xF598: 5 x 0x30), the range flag bytes (0xF590), the
// "default exists" word (0x108D0), GetCPInfo and _setSBUpLow (0x4A16) come through env.
#include <cstdint>
#include <cstring>
uint32_t d2lang_cpToLangId(int cp);   // 0x49BE (EAX = cp; leaves ECX alone)

struct d2lang_mbcp_state
{
	uint32_t lang;          // 0x11AC4
	uint32_t u8;            // 0x11AC8 (untouched)
	uint32_t ismbcp;        // 0x11ACC
	uint8_t uD0[0x10];      // 0x11AD0 (untouched)
	uint8_t ctype[257];     // 0x11AE0: [0], then flags of byte c at [1 + c]
	uint8_t uBE1[3];        // 0x11BE1 (untouched)
	uint32_t cp;            // 0x11BE4
	uint32_t uBE8[2];       // 0x11BE8 (untouched)
	uint32_t w[3];          // 0x11BF0
};
static_assert(sizeof(d2lang_mbcp_state) == 0x138, "0x11AC4..0x11BFC");
struct d2lang_mbcp_entry { uint32_t cp, w[3]; uint8_t ranges[4][8]; };   // 0xF598 + 0x30 i

struct d2lang_mbcp_env
{
	d2lang_mbcp_state* s;
	const uint8_t* flags;               // 0xF590 (4 bytes, one per range list)
	const d2lang_mbcp_entry* tab;       // 0xF598
	const uint32_t* haveDefault;        // 0x108D0
	int (*getCPInfo)(uint32_t cp, uint8_t* info);   // CPINFO (20 bytes): MaxCharSize, DefaultChar[2], LeadByte[12]
	void (*setSBUpLow)();               // 0x4A16
};
static d2lang_mbcp_env d2lang_mbcp_env_g;
extern "C" d2lang_mbcp_env* d2lang_mbcp_getEnv() { return &d2lang_mbcp_env_g; }

// 1.13c D2Lang+0x4BB1
extern "C" int d2lang_crt_setmbcp_4bb1(uint32_t cp)
{
	d2lang_mbcp_env& e = d2lang_mbcp_env_g;
	d2lang_mbcp_state& s = *e.s;
	if (cp != 0)
	{
		for (int i = 0; i < 5; i++)
		{
			const d2lang_mbcp_entry& t = e.tab[i];
			if (t.cp != cp) continue;
			memset(s.ctype, 0, sizeof s.ctype);
			for (int k = 0; k < 4; k++)
				for (const uint8_t* r = t.ranges[k]; r[0] && r[1]; r += 2)
					for (uint32_t c = r[0]; c <= r[1]; c++) s.ctype[1 + c] |= e.flags[k];
			s.cp = cp; s.ismbcp = 1;
			s.lang = d2lang_cpToLangId((int)cp);
			memcpy(s.w, t.w, sizeof s.w);
			e.setSBUpLow();
			return 0;
		}
		uint8_t info[20];
		if (e.getCPInfo(cp, info) == 1)
		{
			uint32_t mx; memcpy(&mx, info, 4);
			memset(s.ctype, 0, sizeof s.ctype);
			s.cp = cp; s.lang = 0;
			if (mx < 2) s.ismbcp = 0;
			else
			{
				const uint8_t* lb = info + 6;
				if (lb[0])
					for (const uint8_t* p = lb + 1; p[0]; p += 2)
					{
						for (uint32_t c = p[-1]; c <= p[0]; c++) s.ctype[1 + c] |= 4;
						if (!p[1]) break;
					}
				for (uint32_t c = 1; c < 0xff; c++) s.ctype[1 + c] |= 8;
				s.lang = d2lang_cpToLangId((int)cp);
				s.ismbcp = 1;
			}
			memset(s.w, 0, sizeof s.w);
			e.setSBUpLow();
			return 0;
		}
		if (*e.haveDefault == 0) return -1;
	}
	memset(s.ctype, 0, sizeof s.ctype);   // _setSBCS (0x49ED)
	s.cp = 0; s.ismbcp = 0; s.lang = 0; memset(s.w, 0, sizeof s.w);
	e.setSBUpLow();
	return 0;
}
