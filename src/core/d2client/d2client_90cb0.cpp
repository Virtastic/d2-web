// 1.13c D2Client+0x90CB0 -- fill a "<selectors>:<text>" format (cdecl out, fmt, arg0, arg1, ..., 0). Nothing without fmt. The args are
// copied into one 2000-char local, 200 chars a slot (retail does not bound their count or length). Without ':' out = fmt. Else, before
// the ':', 'a' and 'n' (first of each, D2Lang strchr) name slots by the digit after them (char - '0'): a only -> 0x8E6D0(slot a, "[ms]");
// n only -> slot n drops its first 4 chars; both -> t = the first 4 chars of slot n when it starts with '[' (slot n then drops them),
// else t = "[ms]", and 0x8E6D0(slot a, t) when slot a starts with '['. out = the text after ':'. Then 0x8E600(EBX out, i, slot i) for every arg.
#include <cstdint>
#include "client_string_algorithms.h"
namespace {
void wcpy(char16_t* d, const char16_t* s) { while ((*d = *s)) { d++; s++; } }
const char16_t* wchr(const char16_t* s, char16_t c) { for (; *s; s++) if (*s == c) return s; return nullptr; }
}
// The same fill on native pointers: n arguments, at most ten.
void d2::client::formatSelectors(char16_t* const o, const char16_t* const f, const char16_t* const* args, int32_t n) {
    if (!f) return;
    char16_t buf[2000] = {};
    for (int32_t i = 0; i < n; i++) wcpy(buf + i * 200, args[i]);
    const char16_t* const colon = wchr(f, u':');
    if (!colon) wcpy(o, f);
    else {
        const char16_t tag[5] = {u'[', u'm', u's', u']', 0};
        int32_t a = -1, m = -1;
        const char16_t* p = wchr(f, u'a');
        if (p && p < colon) a = (int32_t)p[1] - 0x30;
        p = wchr(f, u'n');
        if (p && p < colon) m = (int32_t)p[1] - 0x30;
        if (a == -1) { if (m != -1) wcpy(buf + m * 200, buf + m * 200 + 4); }
        else if (m == -1) d2::client::sub_8E6D0(buf + a * 200, tag);
        else {
            char16_t t[5] = {};
            char16_t* const sn = buf + m * 200;
            if (sn[0] == u'[') {
                for (int i = 0; i < 4; i++) { t[i] = sn[i]; if (!sn[i]) break; }   // D2Lang strncpy, 4 chars
                wcpy(sn, sn + 4);
            } else wcpy(t, tag);
            if (buf[a * 200] == u'[') d2::client::sub_8E6D0(buf + a * 200, t);
        }
        wcpy(o, colon + 1);
    }
    for (int32_t i = 0; i < n; i++) d2::client::sub_8E600(o, (uint32_t)i, buf + i * 200);
}
extern "C" void d2c_90cb0(uint32_t out, uint32_t fmt, const uint32_t* va) {
    if (!fmt) return;
    const char16_t* args[10] = {};
    int32_t n = 0;
    for (; va[n]; n++) args[n] = (const char16_t*)(uintptr_t)va[n];
    d2::client::formatSelectors((char16_t*)(uintptr_t)out, (const char16_t*)(uintptr_t)fmt, args, n);
}
