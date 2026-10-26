// D2Lang batch z27 (Stage-A, undecked). Retail globals/callees are seams; 32-bit links are uint32_t.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

typedef char16_t z27_wc;

// 1.13c D2Lang+0x1204 (__itoa; xtoa body inlined)
extern "C++" char* z27_itoa(int32_t v, char* dst, int32_t radix) {
    bool neg = radix == 10 && v < 0;
    uint32_t u = neg ? 0u - (uint32_t)v : (uint32_t)v;
    char t[40]; int n = 0;
    do { uint32_t d = u % radix; t[n++] = d > 9 ? char(d - 10 + 'a') : char(d + '0'); u /= radix; } while (u);
    char* p = dst; if (neg) *p++ = '-';
    while (n) *p++ = t[--n];
    *p = 0; return dst;
}

// 1.13c D2Lang+0x26E0 (___ascii_stricmp)
extern "C++" int32_t z27_ascii_stricmp(const char* a, const char* b) {
    for (;;) {
        uint8_t x = (uint8_t)*a++, y = (uint8_t)*b++;
        if (x != y) {
            if (x >= 'A' && x <= 'Z') x += 0x20;
            if (y >= 'A' && y <= 'Z') y += 0x20;
            if (x != y) return x < y ? -1 : 1;
        }
        if (!y) return 0;
    }
}

// 1.13c D2Lang+0x11C0 (Unicode default constructor closure)
extern "C++" void z27_unicode_ctor(z27_wc* self) { *self = 0; }

// ---- retail D2Lang globals (DAT_6fc10a60..a88) as one seam struct ----
struct z27_LangState {
    uint32_t list60;                 // DAT_6fc10a60: node chain, next at +8
    int32_t  loaded64, tbl68, tbl6c, tbl70, w74, w78, w7c, w80, w84, refcnt88;
    uint32_t base64, exp84, patch80;  // table handles
    uint32_t (*nextOf)(uint32_t node);
    void (*release)(int32_t id);      // Ordinal_10043(id, 0)
    void (*unloadSysMap)();           // Unicode::unloadSysMap
    uint32_t (*lookup)(uint32_t table, int32_t aux, const uint8_t* key, uint32_t* out); // FUN_6fc09360
    uint32_t (*missing)();            // FUN_6fc09fb0
    const z27_wc* emptyStr;           // DAT_6fc10a98
    void (*fatal)(int32_t msgId);     // Ordinal_10265 + 10024 + _exit(-1)
};

// 1.13c D2Lang+0x9E20 (ordinal 10005, refcounted shutdown)
extern "C++" void z27_shutdown(z27_LangState* s) {
    if (!s->loaded64 || --s->refcnt88 != 0) return;
    if (s->tbl68) { s->release(0x32b); s->w74 = 0; s->release(0x330); s->tbl68 = 0; }
    s->release(0x123); s->loaded64 = 0;
    if (s->tbl6c) { s->release(0x33e); s->tbl6c = 0; }
    if (s->w78) { s->release(0x343); s->w78 = 0; }
    s->release(0x123); s->w80 = 0;
    if (s->tbl70) { s->release(0x34c); s->tbl70 = 0; }
    if (s->w7c) { s->release(0x351); s->w7c = 0; }
    if (s->w84) { s->release(0x123); s->w84 = 0; }
    for (uint32_t n = s->list60; n; ) {
        n = s->nextOf(n);
        s->release(0x379); s->release(0x37a); s->release(0x37b);
    }
    s->list60 = 0;
    s->unloadSysMap();
}

// 1.13c D2Lang+0xA7B0 (ordinal 10010, key -> index)
extern "C++" uint32_t z27_lookup_key(z27_LangState* s, const uint8_t* key, uint32_t* out) {
    if (!s->loaded64) { s->fatal(700); return 0; }
    if (!s->w80) { s->fatal(0x2bd); return 0; }
    if (!key) { s->fatal(0x2c2); return 0; }
    if (!*key) { *out = (uint32_t)(uintptr_t)s->emptyStr; return (uint32_t)(uintptr_t)out & 0xffff0000; }
    uint32_t r = s->lookup(s->patch80, s->tbl6c, key, out);
    if ((int16_t)r != -1) return r + 10000;
    if (s->w84) {
        r = s->lookup(s->exp84, s->tbl70, key, out);
        if ((int16_t)r != -1) return r + 20000;
    }
    r = s->lookup(s->base64, s->tbl68, key, out);
    if ((int16_t)r == -1) { r = s->missing(); *out = r; r &= 0xffff0000; }
    return r;
}

// 1.13c D2Lang+0xA8A0 (ordinal 10011, key -> index only)
extern "C++" uint32_t z27_lookup_index(z27_LangState* s, const uint8_t* key) {
    uint32_t o = 0; z27_lookup_key(s, key, &o); return o;
}

// 1.13c D2Lang+0x9790 (ordinal 10007, u64 with thousands separators and K/M/.. suffix)
struct z27_FmtSeam {
    z27_wc suffix[5];                // DAT_6fc10558..64 for cases 1..4; [0] unused (case 0 = none)
    void (*fatal)(int32_t msgId);
};
extern "C++" void z27_format_u64(const z27_FmtSeam* sm, z27_wc* out, uint64_t val, int32_t cap) {
    auto star = [&] { out[0] = u'*'; out[1] = 0; };
    if (cap < 3) { sm->fatal(0x559); return; }
    char s[64]; snprintf(s, sizeof s, "%llu", (unsigned long long)val);
    int32_t n = (int32_t)strlen(s);
    if (!n) { star(); return; }
    int32_t k = (n - 1) / 3 - 1; if (k > 4) k = 4; if (k < 0) k = 0;
    int32_t rem = n - k * 3, pos = (rem - 1) / 3 + rem;
    if (pos < 1 || cap - 2 <= pos) { star(); return; }
    out[pos + 1] = 0;
    out[pos] = k ? sm->suffix[k] : 0;
    int32_t cnt = 0;
    while (rem) {
        if (!pos) { sm->fatal(0x58e); return; }
        out[--pos] = (z27_wc)(uint8_t)s[--rem];
        if (++cnt > 2) { cnt = 0; if (pos > 0) out[--pos] = u','; }
    }
}

// 1.13c D2Lang+0x8630 (Unicode::isLineBreak); cls = FUN_6fc09290 (language class) via seam
extern "C++" int32_t z27_is_linebreak(const z27_wc* s, uint32_t i, int32_t (*cls)()) {
    if (!i) return 1;
    auto sp = [](z27_wc c) { return c < 0x100 && (c == ' ' || (c >= 9 && c <= 13)); };
    int32_t k = cls();
    if (k == 6 || (k > 7 && k < 10)) {
        z27_wc c = s[i];
        bool keep = c == 0x2026 || c == 0x3001 || c == 0x3002 || c == 0xff01 || c == 0xff0c || c == 0xff0e ||
                    c == 0xff1f || c == 0xff61 || c == 0xff64 || (c >= 'a' && c <= 'z') ||
                    (c >= 0xff41 && c <= 0xff5a) || (c >= 'A' && c <= 'Z') || (c >= 0xff21 && c <= 0xff3a) ||
                    (c >= '0' && c <= '9') || (c >= 0xff10 && c <= 0xff19);
        return keep ? 0 : 1;
    }
    if (sp(s[i])) return 0;
    if (sp(s[i - 1])) return 1;
    return 0;
}

// 1.13c D2Lang+0x8740 (Unicode::Personalize, ordinal 10018)
struct z27_PersSeam {
    bool (*pred)(const char* tbl);                   // FUN_6fc08520
    bool (*hasVowel)(const char* v, const z27_wc* w); // FUN_6fc08420
    void (*win2uni)(z27_wc* out, const char* in, int32_t n); // win2Unicode
    const char* sfx_4a4;   // DAT_6fc0d4a4/8, 5 bytes
    const char* sfx_4ac;   // DAT_6fc0d4ac/ae, 3 bytes
    const char* sfx_49c;   // DAT_6fc0d49c/9e, 3 bytes
    const char* sfx_48c;   // DAT_6fc0d48c/90, 5 bytes
    const char* tbl_4b0; const char* tbl_4a0;
};
static int32_t z27_wlen(const z27_wc* p) { int32_t n = 0; if (p) while (p[n]) n++; return n; }
static void z27_wcat(z27_wc* d, const z27_wc* s) { d += z27_wlen(d); while ((*d++ = *s++)) {} }
extern "C++" void z27_personalize(const z27_PersSeam* sm, z27_wc* out, const z27_wc* a, const z27_wc* b,
                                  int32_t cap, int32_t lang) {
    int32_t la = z27_wlen(a), lb = z27_wlen(b);
    if (cap < lb + 5 + la) {
        if (cap <= lb) return;
        const z27_wc* p = b; z27_wc* d = out; while ((*d++ = *p++)) {}
        return;
    }
    z27_wc t[10] = {0};
    auto put = [&](const char* s, int32_t n) { sm->win2uni(t, s, n); z27_wcat(out, t); };
    auto ch = [&](z27_wc c) { z27_wc x[2] = {c, 0}; z27_wcat(out, x); };
    switch (lang) {
    case 0: case 0xc: case 2: {
        out[0] = 0; z27_wcat(out, a);
        const char* s;
        if (lang == 2) s = sm->pred(sm->tbl_4a0) ? sm->sfx_4ac : sm->sfx_49c;
        else s = sm->pred(sm->tbl_4b0) ? sm->sfx_4ac : "'s ";
        put(s, (int32_t)strlen(s)); z27_wcat(out, b); return;
    }
    case 1: out[0] = 0; z27_wcat(out, b); put(sm->sfx_4a4, (int32_t)strlen(sm->sfx_4a4)); z27_wcat(out, a); return;
    case 3: {
        out[0] = 0; z27_wcat(out, b);
        const char* s = sm->hasVowel("aeiou", a) ? " d'" : sm->sfx_4a4;
        put(s, (int32_t)strlen(s)); z27_wcat(out, a); return;
    }
    case 5: out[0] = 0; z27_wcat(out, b); put(sm->sfx_48c, (int32_t)strlen(sm->sfx_48c)); z27_wcat(out, a); return;
    case 10: out[0] = 0; z27_wcat(out, b); ch('('); z27_wcat(out, a); ch(')'); ch(' '); return;
    default: out[0] = 0; ch('('); z27_wcat(out, a); ch(')'); ch(' '); z27_wcat(out, b); return;
    }
}

// 1.13c D2Lang+0xA9D0 (Unicode::loadSysMap, ordinal 10032). GUESSED: decompile has unaff_ESI/lost-arg
// artifacts; layout inferred: "KAMAP\n" name 0x1a | u16 count | u16 w | u8 n | n wchar | count*{u8,u8,u8,u8} | count u16.
struct z27_SysMap { std::vector<uint8_t> chars; std::vector<uint16_t> pairs, tail; uint16_t count = 0; uint8_t n = 0; char name[0x28] = {0}; };
extern "C++" int32_t z27_load_sysmap(z27_SysMap* m, const uint8_t* d, void (*err)(int32_t)) {
    if (strncmp((const char*)d, "KAMAP\n", 5)) { err(0); return 0; }
    int i = 0;
    while (d[6 + i] != 0x1a) { if (++i > 0x27) { err(0); return 0; } }
    memcpy(m->name, d + 6, i); m->name[i] = 0;
    const uint8_t* p = d + i + 0xb;
    uint16_t cnt = p[0] | (p[1] << 8);
    uint8_t n = p[2];
    m->chars.assign(p + 3, p + 3 + n * 2);
    const uint8_t* q = p + 3 + n * 2;
    m->pairs.clear(); m->tail.clear();
    for (int k = 0; k < cnt; k++, q += 4) { m->pairs.push_back(q[0]); m->pairs.push_back(q[2]); }
    for (int k = 0; k < cnt; k++, q += 2) m->tail.push_back(q[0] | (q[1] << 8));
    m->count = cnt; m->n = n;
    return 1;
}
