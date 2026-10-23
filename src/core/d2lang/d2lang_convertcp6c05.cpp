// 1.13c D2Lang+0x6C05: CRT __convertcp. Win32 calls via env (no direct Win32).
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

struct d2lang_cvcp_env
{
	int (*maxCharSize)(uint32_t cp);  // GetCPInfo: MaxCharSize, 0 on failure
	int (*mbToWc)(uint32_t cp, uint32_t fl, const char* s, int n, uint16_t* d, int dn);
	int (*wcToMb)(uint32_t cp, uint32_t fl, const uint16_t* s, int n, char* d, int dn, const char* def, int* used);
};
static d2lang_cvcp_env d2lang_cvcp_env_g;
extern "C" d2lang_cvcp_env* d2lang_cvcp_getEnv() { return &d2lang_cvcp_env_g; }

// 1.13c D2Lang+0x6C05
extern "C" char* d2lang_cvcp6c05_sub_6C05(uint32_t from, uint32_t to, const char* src, uint32_t* pcb,
                                          char* out, int outn)
{
	d2lang_cvcp_env& e = d2lang_cvcp_env_g;
	uint32_t cb = *pcb;
	if (from == to) return nullptr;
	bool single = e.maxCharSize(from) != 0 && e.maxCharSize(from) == 1 &&
	              e.maxCharSize(to) != 0 && e.maxCharSize(to) == 1;
	int n;
	if (single) n = (cb == 0xffffffffu) ? (int)strlen(src) + 1 : (int)cb;
	else { n = e.mbToWc(from, 1, src, (int)cb, nullptr, 0); if (n == 0) return nullptr; }
	std::vector<uint16_t> w((size_t)n, 0);
	if (e.mbToWc(from, 1, src, (int)cb, w.data(), n) == 0) return nullptr;
	if (out)
		return e.wcToMb(to, 0, w.data(), n, out, outn, nullptr, nullptr) ? out : nullptr;
	int m = n;
	if (!single && (m = e.wcToMb(to, 0, w.data(), n, nullptr, 0, nullptr, nullptr)) == 0) return nullptr;
	char* r = (char*)calloc(1, (size_t)m);
	if (!r) return nullptr;
	uint32_t u = (uint32_t)e.wcToMb(to, 0, w.data(), m, r, m, nullptr, nullptr);
	if (u == 0) { free(r); return nullptr; }
	if (cb != 0xffffffffu) *pcb = u;
	return r;
}
