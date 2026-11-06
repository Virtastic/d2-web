// 1.13c D2Lang+0x6C05: CRT __convertcp. Win32 calls and the CRT heap via env (no direct Win32).
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct d2lang_cvcp_env
{
	int (*getCPInfo)(uint32_t cp, uint32_t* info);  // GetCPInfo: nonzero on success, info[0] = MaxCharSize
	int (*mbToWc)(uint32_t cp, uint32_t fl, const char* s, int n, uint16_t* d, int dn);
	int (*wcToMb)(uint32_t cp, uint32_t fl, const uint16_t* s, int n, char* d, int dn, const char* def, int* used);
	void* (*callocFn)(size_t num, size_t size);     // _calloc 0x3102
	void (*freeFn)(void* p);                        // _free 0x3091
};
static d2lang_cvcp_env d2lang_cvcp_env_g;
extern "C" d2lang_cvcp_env* d2lang_cvcp_getEnv() { return &d2lang_cvcp_env_g; }

// 1.13c D2Lang+0x6C05
// One GetCPInfo per code page; the wide buffer is a zeroed (2n+3)&~3 stack block (retail _alloca; its stack-overflow
// handler falls back to calloc(2, n), which a fixed-size call never reaches). The single-byte path sizes the buffer
// from *pcb (or strlen + 1) and reuses that count for the output.
extern "C" char* d2lang_cvcp6c05_sub_6C05(uint32_t from, uint32_t to, const char* src, uint32_t* pcb,
                                          char* out, int outn)
{
	d2lang_cvcp_env& e = d2lang_cvcp_env_g;
	const uint32_t cb = *pcb;
	if (from == to) return nullptr;
	uint32_t info[5];
	bool single = false;
	if (e.getCPInfo(from, info) && info[0] == 1 && e.getCPInfo(to, info) && info[0] == 1) single = true;
	int n;
	if (single) n = (cb == 0xffffffffu) ? (int)strlen(src) + 1 : (int)cb;
	else { n = e.mbToWc(from, 1, src, (int)cb, nullptr, 0); if (n == 0) return nullptr; }
	const size_t bytes = ((size_t)(uint32_t)n * 2 + 3) & ~(size_t)3;
	uint16_t* w = (uint16_t*)__builtin_alloca(bytes);
	memset(w, 0, (size_t)(uint32_t)n * 2);
	char* r = nullptr;
	if (e.mbToWc(from, 1, src, (int)cb, w, n) == 0) return nullptr;
	if (out)
		return e.wcToMb(to, 0, w, n, out, outn, nullptr, nullptr) ? out : nullptr;
	int m = n;
	if (!single && (m = e.wcToMb(to, 0, w, n, nullptr, 0, nullptr, nullptr)) == 0) return nullptr;
	r = (char*)e.callocFn(1, (size_t)(uint32_t)m);
	if (!r) return nullptr;
	const int u = e.wcToMb(to, 0, w, m, r, m, nullptr, nullptr);
	if (u == 0) { e.freeFn(r); return nullptr; }
	if (cb != 0xffffffffu) *pcb = (uint32_t)u;
	return r;
}
