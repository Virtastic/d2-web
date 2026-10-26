// Batch 45E0: Storm AUVIDEOREC/AUTIMERREC/AUSGDIOBJ allocators, shortsort, locale-string helpers. Retail globals/callees are seams.
#include <cstdint>
#include <cstring>
// ponytail: retail links are 32-bit guest pointers; links are read as uint32_t, the caller owns the native image.

typedef uint32_t *(*StormAllocFn)(uint32_t size, const char *name, int32_t a, uint32_t flags);
typedef void (*StormLinkFn)(void *, void *, uint32_t);

// 1.13c STORM+0x4660 AUVIDEOREC ctor: clear two dwords, run elem ctor over 2 dwords at +0x90.
extern "C" uint32_t *storm_auvideorec_ctor(uint32_t *p, void (*elem_ctor)(void *)) {
    p[0] = 0; p[1] = 0;
    for (int i = 0; i < 2; i++) elem_ctor(p + 0x24 + i);
    return p;
}

// 1.13c STORM+0x45E0 AUVIDEOREC new: alloc(extra+0xa4, "AUVIDEOREC", -2, flags|8), construct, link if link!=0.
extern "C" uint32_t *storm_auvideorec_new(int extra, uint32_t flags, uint32_t link, StormAllocFn alloc, StormLinkFn linkfn,
                                          void (*elem_ctor)(void *)) {
    uint32_t *p = alloc(extra + 0xa4, "AUVIDEOREC", -2, flags | 8);
    uint32_t *r = p ? storm_auvideorec_ctor(p, elem_ctor) : nullptr;
    if (link) linkfn(nullptr, r, link);
    return r;
}

static uint32_t *storm_au2(int extra, uint32_t flags, uint32_t link, StormAllocFn alloc, StormLinkFn linkfn, int sz, const char *nm) {
    uint32_t *p = alloc(extra + sz, nm, -2, flags | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (link) linkfn(nullptr, p, link);
    return p;
}

// 1.13c STORM+0x4CE0 AUTIMERREC new: alloc(extra+0x1c), zero two dwords, link if link!=0.
extern "C" uint32_t *storm_autimerrec_new(int extra, uint32_t flags, uint32_t link, StormAllocFn a, StormLinkFn l) {
    return storm_au2(extra, flags, link, a, l, 0x1c, "AUTIMERREC");
}

// 1.13c STORM+0x4DE0 AUSGDIOBJ new: alloc(extra+0x10), zero two dwords, link if link!=0.
extern "C" uint32_t *storm_ausgdiobj_new(int extra, uint32_t flags, uint32_t link, StormAllocFn a, StormLinkFn l) {
    return storm_au2(extra, flags, link, a, l, 0x10, "AUSGDIOBJ");
}

// 1.13c STORM+0x5CC0 shortsort: selection sort of [lo,hi] (width w); repeatedly swap the max into hi.
extern "C" void storm_shortsort(uint8_t *lo, uint8_t *hi, int w, int (*cmp)(const void *, const void *)) {
    for (; hi > lo; hi -= w) {
        uint8_t *mx = lo;
        for (uint8_t *p = lo + w; p <= hi; p += w)
            if (cmp(p, mx) > 0) mx = p;
        if (mx != hi)
            for (int i = 0; i < w; i++) { uint8_t t = mx[i]; mx[i] = hi[i]; hi[i] = t; }
    }
}

// 1.13c STORM+0x6BE4 __strcats: append n strings to dst via strcat (0x83C0).
extern "C" void storm_strcats(char *dst, int n, const char *const *s) {
    for (int i = 0; i < n; i++) strcat(dst, s[i]);
}

// 1.13c STORM+0x6C08 __lc_strtolc: parse "lang_country.cp" / "lang,.cp" into 3 fields (0x40,0x40,0x10) of a 0x90 buffer. 0 ok, -1 bad.
extern "C" int32_t storm_lc_strtolc(char *out, const char *in) {
    memset(out, 0, 0x90);
    if (!*in) return 0;
    if (in[0] == '.' && in[1]) { strncpy(out + 0x80, in + 1, 0xf); out[0x8f] = 0; return 0; }
    for (uint32_t part = 0;; part++) {
        size_t n = strcspn(in, "_.,");
        if (n == 0) return -1;
        char c = in[n];
        char *dst;
        if (part == 0) { if (n > 0x3f || c == '.') return -1; dst = out; }
        else if (part == 1) { if (n > 0x3f || c == '_') return -1; dst = out + 0x40; }
        else if (part == 2) { if (n > 0xf || (c && c != ',')) return -1; dst = out + 0x80; }
        else return -1;
        strncpy(dst, in, n);
        if (c == ',' || !c) return 0;
        in += n + 1;
    }
}

// 1.13c STORM+0x6CE4 __lc_lctostr: copy the language, then append sep1+country and sep2+codepage when present (seam separators).
extern "C" void storm_lc_lctostr(char *dst, const char *lc, const char *sep1, const char *sep2) {
    strcpy(dst, lc);
    if (lc[0x40]) { strcat(dst, sep1); strcat(dst, lc + 0x40); }
    if (lc[0x80]) { strcat(dst, sep2); strcat(dst, lc + 0x80); }
}

// 1.13c STORM+0x6E47 setlocale core: query/set locale string. cur=0x401e0 str, req=0x40158 str, cp6=0x42478 (6B), v4=0x42480 (4B).
struct StormLocale { char *cur; char *req; uint8_t *cp6; uint8_t *v4; uint32_t *cpsrc; int (*qualify)(char *lc, void *cp); };
extern "C" char *storm_setlocale_core(StormLocale *g, const char *name, char *out, uint16_t *o6, uint32_t *o4) {
    if (!name) return nullptr;
    if (name[0] == 'C' && !name[1]) {
        out[0] = 'C'; out[1] = 0;
        if (o6) o6[0] = o6[1] = o6[2] = 0;
        if (o4) *o4 = 0;
        return out;
    }
    if (strlen(name) > 0x81 || (strcmp(g->cur, name) && strcmp(g->req, name))) {
        char lc[0x90];
        if (storm_lc_strtolc(lc, name) || !g->qualify(lc, g->cp6)) return nullptr;
        memcpy(g->v4, g->cpsrc, 4);
        storm_lc_lctostr(g->cur, lc, "_", ".");
        if (!*name || strlen(name) > 0x81) name = "";
        g->req[0x82] = 0;
        strncpy(g->req, name, 0x82);
    }
    if (o6) memcpy(o6, g->cp6, 6);
    if (o4) memcpy(o4, g->v4, 4);
    strcpy(out, g->cur);
    return g->cur;
}
