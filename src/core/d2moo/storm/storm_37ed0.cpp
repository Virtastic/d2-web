// 1.13c Storm+0x37ED0 (ordinal 312), from the cached decompile and the function's disassembly.
// Prior art used: none (D2MOO's Storm module has no body for this ordinal).
// The three scratch strings sit in one frame as retail lays them out (control string at +0x08, name template at +0x18,
// output at +0x28), so reads past a string's end land on the same neighbour bytes. The frame starts zeroed.
// Retail passes EAX = &id and EDX = &cap (its slot zeroed first) to 0x37E80; the id it rewrites is the one formatted,
// and the capacity is the value read on entry.
#include "storm_37ed0.h"
#include <cstring>

namespace storm_37ed0 {
// 1.13c Storm+0x37ED0
bool Storm_37ED0(storm25220::State& s, const Tables& t, uint32_t id, char* dst, uint32_t cap, uint32_t flag) {
    if (id == 0 || dst == nullptr || cap == 0) {
        s.error = 0x57;
        s.setLastError(0x57);
        return false;
    }
    if (flag != 0) {
        uint32_t slot = 0;
        int32_t code = (int32_t)id;
        t.sub37E80(&code, &slot);
        id = (uint32_t)code;
    }
    char fr[0x100];
    std::memset(fr, 0, sizeof(fr));
    char* buf = fr + 0x08;   // the control string
    char* tmpl = fr + 0x18;  // the name template
    char* out = fr + 0x28;   // the output string
    storm25220::copy(s, tmpl, (const char*)t.scd + (id & 3) + ((id >> 2) & 7) * 0x10, 0x10);
    buf[0] = (char)t.ctl[(id >> 6) & 3];
    buf[1] = (char)t.ctl[(id >> 8) & 3];
    buf[2] = (char)t.ctl[(id >> 10) & 3];
    buf[3] = (char)t.ctl[(id >> 12) & 3];
    buf[4] = (char)t.ctl[(id >> 14) & 3];
    buf[5] = 0;
    if (id & 0x20) storm25220::append(s, buf, (const char*)t.xform, 0x10);
    // drop trailing "~~" pairs: the three bytes from len-2 (the NUL included) compared with "~~\0"
    if (storm25220::length(s, buf) >= 2) {
        do {
            const char* p = buf - 2 + storm25220::length(s, buf);
            if (!(p[0] == '~' && p[1] == '~' && p[2] == '\0')) break;
            buf[storm25220::length(s, buf) - 2] = 0;
        } while (storm25220::length(s, buf) >= 2);
    }
    int32_t n = 0;
    for (const char* c = buf; *c; ++c)
        if (*c != '~') ++n;
    ++n;
    for (int32_t i = 0; i < n; ++i)
        if (tmpl[i] == 'A') ++n;
    std::memcpy(out, &t.prefix, 4);
    out[4] = (char)t.prefixByte;
    char* o = out + storm25220::length(s, out);
    *o++ = tmpl[n - 1];
    --n;
    bool usedA = false;
    for (const char* c = buf; *c;) {
        if (*c == '~') {
            *o++ = '^';
            *o++ = '1';
            ++c;
        } else if (tmpl[n - 1] == 'A' && !usedA) {
            const char ch = tmpl[n - 2];
            out[2] = 'A';
            *o++ = ' ';
            *o++ = 'D';
            *o++ = '=';
            n -= 2;
            *o++ = ch;
            usedA = true;
        } else {
            *o++ = *c++;
            *o++ = tmpl[n - 1];
            --n;
        }
    }
    *o = 0;
    storm25220::copy(s, dst, out, cap);
    return cap >= (uint32_t)(o - out) + 1;
}
} // namespace storm_37ed0
