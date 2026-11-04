// Weak link-time stand-ins for seams whose owning rows are not merged yet (batch 105-108 dropped branches).
// Weak: the real definition wins the moment its branch lands. Until then a call traps nothing and returns 0.
// Not parity evidence; delete an entry when its row is merged.
#include <cstdint>
#include <cstddef>
#define W __attribute__((weak))
extern "C" {
// D2Win
W uint8_t d2win_g_cptab[512], d2win_g_cpbits[4], d2win_g_mbctype[0x101], d2win_g_mbf[3 * 4];
W uint32_t d2win_g_mbcp, d2win_g_mbcinit, d2win_g_mblcid;
W int32_t d2win_g_sbcsok, g_d2win_mouse_x_1488, g_d2win_mouse_y_148c;
W char d2win_g_decpt;
W void d2win_setSBCS(void) {}
W void d2win_sub_57A8(void) {}
W uint32_t d2win_sub_13da0(void*, int32_t, int32_t) { return 0; }
W void d2win_sub_1978A(const char*, int32_t, uint32_t*) {}
W void d2win_sub_1A184(uint32_t*, int32_t, int32_t) {}
W void d2win_sub_19F4E(int32_t*, int32_t*) {}
W int32_t d2win_f410_pumpDepth() { return 0; }
// Fog
W void fog_sub_15f50(void*, uint32_t, void*) {}
W void FUN_6ff5fc20() {}
W void FUN_6ff60f30() {}
W void FUN_6ff62020() {}
W void FUN_6ff684a0() {}
W void FUN_6ff6cda0() {}
W void Ordinal_116() {}
void Fog_Ordinal_501_SStrCopy(char*, const char*, uint32_t);
W void Ordinal_501(char* d, const char* s, uint32_t n) { Fog_Ordinal_501_SStrCopy(d, s, n); }
W void Storm_Ordinal_501(void* d, const void* s, int32_t n) { Fog_Ordinal_501_SStrCopy((char*)d, (const char*)s, (uint32_t)n); }
// Multiplayer batch helpers (Fog/Storm string imports and the roster count); weak, real definitions win.
W int32_t DAT_6fa09efc;
W int __stricmp(const char* a, const char* b) { for (;; a++, b++) { int x = (unsigned char)*a, y = (unsigned char)*b; if (x >= 'A' && x <= 'Z') x += 32; if (y >= 'A' && y <= 'Z') y += 32; if (x != y) return x < y ? -1 : 1; if (!x) return 0; } }
W size_t ustrlen(const uint16_t* s) { size_t n = 0; while (s[n]) n++; return n; }
W uint16_t* ustrcpy(uint16_t* d, const uint16_t* s) { uint16_t* r = d; while ((*d++ = *s++)) {} return r; }
W uint16_t* ustrcat(uint16_t* d, const uint16_t* s) { ustrcpy(d + ustrlen(d), s); return d; }
W uint16_t* ustrncat(uint16_t* d, const uint16_t* s, int32_t n) { uint16_t* e = d + ustrlen(d); while (n-- > 0 && *s) *e++ = *s++; *e = 0; return d; }
W void utoUnicode(uint16_t* d, const char* s, int32_t n) { while (n-- > 0 && *s) *d++ = (uint8_t)*s++; *d = 0; }
W void uwin2Unicode(uint16_t* d, const char* s, int32_t n) { while (n-- > 0 && *s) *d++ = (uint8_t)*s++; *d = 0; }
W char* Ordinal_571(const char* s, int32_t c) { if (!s) return 0; for (;; s++) { if (*s == (char)c) return (char*)s; if (!*s) return 0; } }
W int32_t Storm_Ordinal_506(const void* s) { const char* p = (const char*)s; int32_t n = 0; while (p && p[n]) n++; return n; }
W uint8_t DAT_6ff76690[0x100], DAT_6ff768ec[0x100];
// Fog/Storm ordinal imports no merged row defines yet (link fix); signatures match the importers.
extern uint32_t Fog_Ordinal_10265(uint32_t); extern void Fog_Ordinal_10024(const void*, const void*, uint32_t); extern void Fog_Ordinal_10043(int32_t, int32_t);
W uint32_t Ordinal_10265(uint32_t a) { return Fog_Ordinal_10265(a); }
W void Ordinal_10024(const void* m, uint32_t l) { Fog_Ordinal_10024(m, 0, l); }
W int32_t Ordinal_10107(int32_t) { return 0; }
W void Ordinal_10109(void) {}
W void Ordinal_10043(int32_t a, int32_t b) { Fog_Ordinal_10043(a, b); }
W int wsprintfA(char*, const char*, ...) { return 0; }
}
