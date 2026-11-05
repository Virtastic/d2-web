// 1.13c D2Client+0x8E600 -- substitute the first "%<digit>" of a string (EBX = string, rewritten in place; [esp+4] = the digit as a
// number, [esp+8] = insert). The chars before the first '%' followed by '0' + digit are copied to a zeroed 400-char local (only the
// first len - 1 positions are examined; no match leaves the string alone). On a match: local += insert, then the text after the
// marker -- one char further when the local now ends in ' ' and a ' ' follows the marker -- and string = local.
#include "cl_env.h"
namespace {
void wcat(char16_t* d, const char16_t* s) { while (*d) d++; while ((*d = *s)) { d++; s++; } }
int32_t wlen(const char16_t* s) { int32_t n = 0; while (s[n]) n++; return n; }
}
extern "C" void d2c_8e600(uint32_t str, int32_t digit, uint32_t ins) {
    char16_t* s = (char16_t*)(uintptr_t)str;
    char16_t tmp[400];
    for (int i = 0; i < 400; i++) tmp[i] = 0;
    uint32_t want = (uint32_t)(digit + 0x30);
    int32_t last = wlen(s) - 1, i = 0;
    for (; i < last; i++) {
        if (s[i] == u'%' && (uint32_t)s[i + 1] == want) break;
        tmp[i] = s[i];
    }
    if (i >= last) return;
    wcat(tmp, (const char16_t*)(uintptr_t)ins);
    int32_t m = wlen(tmp);
    wcat(tmp, s + i + ((m > 0 && tmp[m - 1] == u' ' && s[i + 2] == u' ') ? 3 : 2));
    char16_t* d = s;
    const char16_t* p = tmp;
    while ((*d = *p)) { d++; p++; }
}
