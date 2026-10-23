// D2Lang batch s-d2lang-next-30: CRT _realloc (0x4FD3) and _setmbcp (0x4BB1).
#include <cstdint>
#include <cstdlib>
#include <cstring>
uint32_t d2lang_cpToLangId(int cp);

// 1.13c D2Lang+0x4FD3: _realloc(p, n). NULL p -> malloc; n==0 -> free, NULL. Retail's small-block-heap / HeapReAlloc /
// new-handler retry paths collapse to host realloc (single heap on wasm); size > 0xFFFFFFE0 fails.
void* d2lang_crt_realloc_4fd3(void* p, uint32_t n) {
    if (!p) return malloc(n);
    if (!n) { free(p); return nullptr; }
    if (n > 0xffffffe0u) return nullptr;
    return realloc(p, n);
}

// 1.13c D2Lang+0x4BB1: _setmbcp(cp). Rebuilds the 257-byte mbctype table (g11ae0, [0] cleared, flags at [1+c]) and sets
// cp/ismbcp/lang/extra words. Retail's builtin table (0x6fc0f598, 5 entries x 0x30: cp, 3 words, 4 range lists of 8 bytes)
// and the flag bytes (0x6fc0f590) are passed in. Returns 0, or -1 if cp is unknown to table and OS and no fallback.
struct MbcpEntry { uint32_t cp, w[3]; uint8_t ranges[4][8]; };
struct MbcpState { uint8_t ctype[257]; uint32_t cp, ismbcp, lang, w[3]; };
// os_lead: GetCPInfo lead-byte pairs (NULL = GetCPInfo failed), os_max: MaxCharSize.
int d2lang_crt_setmbcp_4bb1(MbcpState& s, uint32_t cp, const MbcpEntry* tab, const uint8_t flag[4],
                            const uint8_t* os_lead, uint32_t os_max, bool have_default) {
    auto fin = [&] { return 0; };
    auto sbcs = [&] { memset(s.ctype, 0, sizeof s.ctype); s.cp = 0; s.ismbcp = 0; s.lang = 0; memset(s.w, 0, sizeof s.w); return fin(); };
    if (!cp) return sbcs();
    for (int i = 0; i < 5 && tab; i++) {
        if (tab[i].cp != cp) continue;
        memset(s.ctype, 0, sizeof s.ctype);
        for (int k = 0; k < 4; k++)
            for (const uint8_t* r = tab[i].ranges[k]; r[0] && r[1]; r += 2)
                for (uint32_t c = r[0]; c <= r[1]; c++) s.ctype[1 + c] |= flag[k];
        s.cp = cp; s.ismbcp = 1; s.lang = d2lang_cpToLangId(cp);
        memcpy(s.w, tab[i].w, sizeof s.w);
        return fin();
    }
    if (os_lead || os_max) {
        memset(s.ctype, 0, sizeof s.ctype);
        s.cp = cp; s.lang = 0; memset(s.w, 0, sizeof s.w);
        if (os_max < 2) { s.ismbcp = 0; return fin(); }
        for (const uint8_t* r = os_lead; r && r[0] && r[1]; r += 2)
            for (uint32_t c = r[0]; c <= r[1]; c++) s.ctype[1 + c] |= 4;
        for (uint32_t c = 1; c < 0xff; c++) s.ctype[1 + c] |= 8;
        s.lang = d2lang_cpToLangId(cp);
        s.ismbcp = 1;
        return fin();
    }
    if (!have_default) return -1;
    return sbcs();
}
