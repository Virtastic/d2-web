// D2Lang batch b75: the one portable row (heap-free of a threadlocinfo). Others are SEH/Win32/CS.
#include <cstdint>
#include <cstdlib>
#include <initializer_list>

extern "C" {
__attribute__((weak)) void d2lang_free_lconv_num(void* lc);
__attribute__((weak)) void d2lang_free_lconv_mon(void* lc);
__attribute__((weak)) void D2Lang_FreeLcTime(void** p);
}
// Retail globals compared against (C-locale defaults); weak so link never needs them.
__attribute__((weak)) extern void* g_lcMonDefault;   // DAT_6fc10864
__attribute__((weak)) extern int* g_lcGroupMonC;     // DAT_6fc11d04
__attribute__((weak)) extern int* g_lcGroupNumC;     // DAT_6fc11d08
__attribute__((weak)) extern int* g_lcCtypeC;        // DAT_6fc11d00
__attribute__((weak)) extern void* g_lcTimeC;        // DAT_6fc10860

// Field order follows retail offsets 0x2c..0x50 (pointer slots).
struct D2LangTlocinfo {
    uint8_t pad0[0x2c];
    int** refcnt2c; int* grp30; int* grp34; void* pad38; void* lconv3c;
    int* ctype40; uint8_t* ctype44; uint8_t pad48[8]; void** time50;
};

// 1.13c D2Lang+0x1C9C (___freetlocinfo)
void d2lang_freetlocinfo(D2LangTlocinfo* p)
{
    if (p->lconv3c != g_lcMonDefault && p->lconv3c && **p->refcnt2c == 0) {
        int* g = p->grp34;
        if (g && *g == 0 && g != g_lcGroupMonC) { free(g); d2lang_free_lconv_mon(p->lconv3c); }
        g = p->grp30;
        if (g && *g == 0 && g != g_lcGroupNumC) { free(g); d2lang_free_lconv_num(p->lconv3c); }
        free(p->refcnt2c);
        free(p->lconv3c);
    }
    int* c = p->ctype40;
    if (c != g_lcCtypeC && c && *c == 0) { free(c); free(p->ctype44 - 0xfe); }
    void** t = p->time50;
    if (t != g_lcTimeC && t && reinterpret_cast<int*>(t)[0x2d] == 0) { D2Lang_FreeLcTime(t); free(p->time50); }
    free(p);
}
