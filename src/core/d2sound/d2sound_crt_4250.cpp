// MSVC CRT locale/memcpy shims, D2sound 1.13c. free() and the C-locale default tables are host seams.
#include <cstdint>
#include <cstring>

// 1.13c D2sound+0x04250 -- _memcpy: overlap-safe (backward copy when dst lies inside src..src+n), returns dst
void* D2sound_4250(void* dst, const void* src, uint32_t n) { return memmove(dst, src, n); }

// 1.13c D2sound+0x047F3 -- ___free_lc_time: free 43 string slots, in retail order (slot 0 after 6, slot 7 after 0xd)
void D2sound_47F3(void** t, void (*fre)(void*)) {
    if (!t) return;
    static const uint8_t order[43] = {1,2,3,4,5,6,0,8,9,10,11,12,13,7,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42};
    for (uint8_t i : order) fre(t[i]);
}

// free t[from..from+n) unless it equals the C-locale slot (cLocale[from+i]) or the static default (dflt[i])
static void freeConv(void** t, const void* const* cLocale, const void* const* dflt, int from, int n, void (*fre)(void*)) {
    for (int i = 0; i < n; i++)
        if (t[from + i] != cLocale[from + i] && t[from + i] != dflt[i]) fre(t[from + i]);
}

// 1.13c D2sound+0x04983 -- ___free_lconv_num: slots 0..2 (decimal_point, thousands_sep, grouping)
void D2sound_4983(void** t, const void* const* cLocale, const void* const* dflt, void (*fre)(void*)) {
    if (t) freeConv(t, cLocale, dflt, 0, 3, fre);
}

// 1.13c D2sound+0x049E2 -- ___free_lconv_mon: slots 3..9 (dflt indexed from 0 = slot 3)
void D2sound_49E2(void** t, const void* const* cLocale, const void* const* dflt, void (*fre)(void*)) {
    if (t) freeConv(t, cLocale, dflt, 3, 7, fre);
}
