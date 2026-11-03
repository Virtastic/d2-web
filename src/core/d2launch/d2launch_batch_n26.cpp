#include <cstdint>
// 1.13c D2Launch+0x14710 name accepted iff 1<len<16 and no '_'/'-' at either end
bool d2launch_charname_ok(const char* s, int32_t len) {
    return len > 1 && len < 16 && s[len-1] != '_' && s[0] != '_' && s[len-1] != '-' && s[0] != '-';
}
// 1.13c D2Launch+0x14710 (core d2l_14710_core in d2launch_14710.cpp)
#include <cstddef>
extern "C" {
__attribute__((weak)) const void* Ordinal_10166(uint32_t ecx);
__attribute__((weak)) int d2l_unicode_strlen(const void*);
__attribute__((weak)) void d2l_unicode_toUtf(char*, const void*, uint32_t);
__attribute__((weak)) int FUN_6fa52340(const char* esiUtf);
__attribute__((weak)) void Ordinal_10147(uint32_t ecx, uint32_t edx);
__attribute__((weak)) extern uint32_t DAT_6fa653b8, DAT_6fa65298;
struct D2l14710Seams { const void* (*text10166)(uint32_t); int (*ustrlen)(const void*); void (*toUtf)(char*, const void*, uint32_t); int (*f12340)(const char*); void (*apply10147)(uint32_t, uint32_t); };
void d2l_14710_core(uint32_t, uint32_t, const D2l14710Seams*);
void FUN_6fa54710() {
    static const D2l14710Seams S = { Ordinal_10166, d2l_unicode_strlen, d2l_unicode_toUtf, FUN_6fa52340, Ordinal_10147 };
    d2l_14710_core(DAT_6fa653b8, DAT_6fa65298, &S);
}
}
