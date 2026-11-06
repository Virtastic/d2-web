// Weak link-time stand-ins for seams whose owning rows are not merged yet (batch 105-108 dropped branches).
// Weak: the real definition wins the moment its branch lands. Until then a call traps nothing and returns 0.
// Not parity evidence; delete an entry when its row is merged.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#define W __attribute__((weak))
char* Storm_Sub3BB00_z14(char*, char, int);
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
W void Ordinal_10167(void) {}
W int32_t DAT_6fa09efc;
W uint8_t DAT_6f9ebf20[0x140];
// Fog
W void fog_sub_15f50(void*, uint32_t, void*) {}
W void FUN_6ff5fc20() {}
W void FUN_6ff60f30() {}
W void FUN_6ff62020() {}
W void* FUN_6ff520d7(void* p, size_t n) { return realloc(p, n); }  // CRT realloc stand-in (link fix)
W uint32_t* FUN_6ff5e4e0(uint8_t*, uint32_t) { return nullptr; }  // roster lookup stand-in (link fix)
W int Ordinal_3(int) { return 0; }  // Storm ordinal stand-in (link fix)
W void FUN_6ff6b0c0(void**) {}
W void Ordinal_111(void) {}
W int Ordinal_19(int) { return 0; }
W void FUN_6ff6ad20(const char*) {}
W void* FUN_6ff6cd10(int, const char*, int) { return nullptr; }
W int FUN_6ff5ceb0(uint32_t) { return 0; }
W void FUN_6ff5d230(void) {}
W void FUN_6ff5e1b0(uint32_t) {}
W void FUN_6ff5e3b0(void) {}
W void FUN_6ff690a0(const char*, const char*) {}
W int FUN_6ff70180(int*, uint32_t*, uint32_t) { return 0; }
W void FUN_6ff5e420(void) {}
W long InterlockedDecrement(volatile long* p) { return __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST); }
W long InterlockedIncrement(volatile long* p) { return __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST); }
W void Sleep(uint32_t) {}  // single-threaded host: a yield is a no-op
W void EnterCriticalSection(void*) {}
W void LeaveCriticalSection(void*) {}
W void DeleteCriticalSection(void*) {}
W void FUN_6ff684a0() {}
W void FUN_6ff6cda0() {}
W void Ordinal_116() {}
void Fog_Ordinal_501_SStrCopy(char*, const char*, uint32_t);
W void Ordinal_501(char* d, const char* s, uint32_t n) { Fog_Ordinal_501_SStrCopy(d, s, n); }
W void Storm_Ordinal_501(void* d, const void* s, int32_t n) { Fog_Ordinal_501_SStrCopy((char*)d, (const char*)s, (uint32_t)n); }
// Multiplayer batch helpers (Fog/Storm string imports and the roster count); weak, real definitions win.
W size_t ustrlen(const uint16_t* s) { size_t n = 0; while (s[n]) n++; return n; }
W uint16_t* ustrcpy(uint16_t* d, const uint16_t* s) { uint16_t* r = d; while ((*d++ = *s++)) {} return r; }
W uint16_t* ustrcat(uint16_t* d, const uint16_t* s) { ustrcpy(d + ustrlen(d), s); return d; }
W uint16_t* ustrncat(uint16_t* d, const uint16_t* s, int32_t n) { uint16_t* e = d + ustrlen(d); while (n-- > 0 && *s) *e++ = *s++; *e = 0; return d; }
W void utoUnicode(uint16_t* d, const char* s, int32_t n) { while (n-- > 0 && *s) *d++ = (uint8_t)*s++; *d = 0; }
W void uwin2Unicode(uint16_t* d, const char* s, int32_t n) { while (n-- > 0 && *s) *d++ = (uint8_t)*s++; *d = 0; }
W void Ordinal_10097(void) {}
W int32_t Ordinal_10098(const void*) { return 0; }
W intptr_t Ordinal_10020(intptr_t) { return 0; }
W void d2_multi_exit(int32_t) {}
W uint8_t DAT_6f9ea2a0[0x100];
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

// Storm #571 (SStrChr, first occurrence): the port is C++-linkage.
char* Storm_Sub3BB00_z14(char*, char, int);
extern "C" W char* Ordinal_571(const char* s, int32_t c) { return Storm_Sub3BB00_z14((char*)s, (char)c, 0); }

// CRT __stricmp (ASCII path): the port is d2_client_39c9.
extern "C" int32_t d2_client_39c9(const char*, const char*);
extern "C" W int __stricmp(const char* a, const char* b) { return d2_client_39c9(a, b); }
