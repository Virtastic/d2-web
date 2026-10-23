// Weak link-time stand-ins for seams whose owning rows are not merged yet (batch 105-108 dropped branches).
// Weak: the real definition wins the moment its branch lands. Until then a call traps nothing and returns 0.
// Not parity evidence; delete an entry when its row is merged.
#include <cstdint>
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
W void FUN_6ff6b560() {}
W void FUN_6ff6cda0() {}
W void Ordinal_116() {}
W void Ordinal_501() {}
W uint8_t DAT_6ff76690[0x100], DAT_6ff768ec[0x100];
// Fog/Storm ordinal imports no merged row defines yet (link fix); signatures match the importers.
W uint32_t Ordinal_10265(uint32_t) { return 0; }
W void Ordinal_10024(const void*, uint32_t) {}
W int32_t Ordinal_10107(int32_t) { return 0; }
W void Ordinal_10109(void) {}
W void* Ordinal_10042(int32_t, int32_t) { return nullptr; }
W void Ordinal_10043(int32_t, int32_t) {}
W int wsprintfA(char*, const char*, ...) { return 0; }
}
