// src/core/fog/fog_n61.cpp -- 1.13c Fog.dll statically linked MSVC CRT: LCMapStringA and _output.
// Win32 / CRT-global dependencies are host seams passed as arguments.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

// 1.13c FOG+0x03279 -- CRT __crtLCMapStringA. Seams: lcmap_w = LCMapStringW, mb2wc/wc2mb = the Win32
// converters, lcmap_a = LCMapStringA, cp_for_lcid = FUN 0x86E0 (0xFFFFFFFF = unknown), recode = FUN
// 0x8727 (src_cp,dst_cp,src,&len,out,outlen -> malloc'd buffer or null; updates len).
// api_kind is DAT 0x80260 (0 unknown, 1 W available, 2 W unavailable); *api_kind is probed in place.
struct FogLcmapSeams {
    void* ctx;
    int (*lcmap_w)(void*, uint32_t lcid, uint32_t flags, const uint16_t* s, int n, uint16_t* d, int dn);
    int (*mb2wc)(void*, uint32_t cp, uint32_t fl, const char* s, int n, uint16_t* d, int dn);
    int (*wc2mb)(void*, uint32_t cp, uint32_t fl, const uint16_t* s, int n, char* d, int dn);
    int (*lcmap_a)(void*, uint32_t lcid, uint32_t flags, const char* s, int n, char* d, int dn);
    uint32_t (*cp_for_lcid)(void*, uint32_t lcid);
    char* (*recode)(void*, uint32_t src_cp, uint32_t dst_cp, const char* s, int* len, char* out, int outlen);
    int (*last_error_is_not_impl)(void*);  // GetLastError()==0x78 after the probe
    int* api_kind;
    uint32_t default_lcid, default_cp;
};

extern "C" uint32_t fog_crt_lcmapstringa_3279(const FogLcmapSeams* h, uint32_t lcid, uint32_t flags, const char* src,
                                              int srclen, char* dst, int dstlen, uint32_t cp, int err_on_bad) {
    static const uint16_t kEmpty[1] = {0};
    if (*h->api_kind == 0) {
        if (h->lcmap_w(h->ctx, 0, 0x100, kEmpty, 1, nullptr, 0) == 0) {
            if (h->last_error_is_not_impl(h->ctx)) *h->api_kind = 2;
        } else *h->api_kind = 1;
    }
    if (srclen > 0) {  // bounded strlen
        int n = 0;
        while (n < srclen && src[n]) ++n;
        srclen = n;
    }
    if (*h->api_kind == 1) {
        if (cp == 0) cp = h->default_cp;
        int wn = h->mb2wc(h->ctx, cp, err_on_bad ? 9 : 1, src, srclen, nullptr, 0);
        if (!wn) return 0;
        std::vector<uint16_t> w(wn);
        if (!h->mb2wc(h->ctx, cp, 1, src, srclen, w.data(), wn)) return 0;
        int r = h->lcmap_w(h->ctx, lcid, flags, w.data(), wn, nullptr, 0);
        if (!r) return 0;
        if (flags & 0x400) {
            if (dstlen && r <= dstlen) h->lcmap_w(h->ctx, lcid, flags, w.data(), wn, (uint16_t*)dst, dstlen);
            return (uint32_t)r;
        }
        std::vector<uint16_t> m(r);
        if (!h->lcmap_w(h->ctx, lcid, flags, w.data(), wn, m.data(), r)) return 0;
        return (uint32_t)h->wc2mb(h->ctx, cp, 0, m.data(), r, dstlen ? dst : nullptr, dstlen);
    }
    if (*h->api_kind != 2) return 0;
    if (lcid == 0) lcid = h->default_lcid;
    if (cp == 0) cp = h->default_cp;
    uint32_t lcp = h->cp_for_lcid(h->ctx, lcid);
    if (lcp == 0xFFFFFFFFu) return 0;
    if (lcp == cp) return (uint32_t)h->lcmap_a(h->ctx, lcid, flags, src, srclen, dst, dstlen);
    char* in = h->recode(h->ctx, cp, lcp, src, &srclen, nullptr, 0);
    if (!in) return 0;
    int n = h->lcmap_a(h->ctx, lcid, flags, in, srclen, nullptr, 0);
    uint32_t ret = 0;
    char* back = nullptr;
    if (n) {
        std::vector<char> tmp(n, 0);
        n = h->lcmap_a(h->ctx, lcid, flags, in, srclen, tmp.data(), n);
        if (n) {
            back = h->recode(h->ctx, lcp, cp, tmp.data(), &n, dst, dstlen);
            ret = back ? (uint32_t)n : 0;
        }
    }
    free(in);
    if (back && back != dst) free(back);
    return ret;
}

// 1.13c FOG+0x0411F -- CRT _output (printf core). args = the va_list as 32-bit slots; pointer slots go
// through ptr(); the float hooks are __fptrap +0x7eaa8/+0x7eab4/+0x7eaac (cvt, forcdecpt, cropzeros).
struct FogOutSeams {
    void* ctx;
    int (*put)(void*, char);                              // write one byte; 0 on stream error
    const void* (*ptr)(void*, uint32_t slot);             // resolve a pointer argument
    int (*wctomb)(void*, char* out, uint16_t wc);         // <0 on failure
    int (*is_lead)(void*, uint8_t c);                     // ctype lead-byte flag (0x80)
    void (*write_count)(void*, uint32_t slot, int n, int is_short);  // %n
    void (*cvt)(void*, const uint32_t* dbl, char* buf, int ch, int prec, int capexp);
    void (*forcdecpt)(void*, char* buf);
    void (*cropzeros)(void*, char* buf);
};

// Class table (RVA 0x25650-0x20) and state table, 1.13c bytes.
static const uint8_t kOutTbl[0x90] = {
    0x06,0x00,0x00,0x06,0x00,0x01,0x00,0x00,0x10,0x00,0x03,0x06,0x00,0x06,0x02,0x10,0x04,0x45,0x45,0x45,0x05,0x05,0x05,0x05,
    0x05,0x35,0x30,0x00,0x50,0x00,0x00,0x00,0x00,0x20,0x28,0x38,0x50,0x58,0x07,0x08,0x00,0x37,0x30,0x30,0x57,0x50,0x07,0x00,
    0x00,0x20,0x20,0x08,0x00,0x00,0x00,0x00,0x08,0x60,0x68,0x60,0x60,0x60,0x60,0x00,0x00,0x70,0x70,0x78,0x78,0x78,0x78,0x08,
    0x07,0x08,0x00,0x00,0x07,0x00,0x08,0x08,0x08,0x00,0x00,0x08,0x00,0x08,0x00,0x07,0x08};

extern "C" int fog_crt_output_411F(const FogOutSeams* h, const uint8_t* fmt, const uint32_t* args) {
    int count = 0, state = 0;
    static const uint16_t kNullW[7] = {'(', 'n', 'u', 'l', 'l', ')', 0};
    static const char kNull[] = "(null)";
    int width = 0, prec = -1, capexp = 0, no_out = 0, wide = 0, prefixlen = 0, hexadd = 0;
    uint32_t flags = 0;
    char prefix[2] = {0, 0};
    const void* text = nullptr;
    int textlen = 0;
    char numbuf[0x230];
    std::vector<char> fbuf;
    auto wc = [&](char c) { if (count >= 0) { if (h->put(h->ctx, c)) ++count; else count = -1; } };
    auto multi = [&](char c, int n) { while (n-- > 0 && count >= 0) wc(c); };
    auto str = [&](const char* p, int n) { while (n-- > 0 && count >= 0) wc(*p++); };
    auto arg = [&](int words) { const uint32_t* a = args; args += words; return a; };
    uint8_t ch = *fmt;
    while (ch && count >= 0) {
        const uint8_t* next = fmt + 1;
        int cls = (ch < 0x20 || ch > 0x78) ? 0 : (kOutTbl[ch - 0x20] & 0xf);
        state = (int8_t)kOutTbl[cls * 8 + state] >> 4;
        bool again = true;
        while (again) {
            again = false;
            switch (state) {
            case 0:
                no_out = 0;  // per-char literal; the original reuses local_240 as the wide-text flag
                wide = 0;
                if (h->is_lead(h->ctx, ch)) { wc((char)ch); next = fmt + 2; wc((char)fmt[1]); }
                else wc((char)ch);
                break;
            case 1: prec = -1; capexp = 0; no_out = 0; width = 0; prefixlen = 0; flags = 0; wide = 0; prefix[0] = prefix[1] = 0; break;
            case 2:
                if (ch == ' ') flags |= 2; else if (ch == '#') flags |= 0x80; else if (ch == '+') flags |= 1;
                else if (ch == '-') flags |= 4; else if (ch == '0') flags |= 8;
                break;
            case 3:
                if (ch == '*') { width = (int32_t)*arg(1); if (width < 0) { flags |= 4; width = -width; } }
                else width = (char)ch - '0' + width * 10;
                break;
            case 4: prec = 0; break;
            case 5:
                if (ch == '*') { prec = (int32_t)*arg(1); if (prec < 0) prec = -1; }
                else prec = (char)ch - '0' + prec * 10;
                break;
            case 6:
                if (ch == 'I') {
                    uint8_t b = fmt[1];
                    if (b == '6' && fmt[2] == '4') { next = fmt + 3; flags |= 0x8000; }
                    else if (b == '3' && fmt[2] == '2') { next = fmt + 3; flags &= ~0x8000u; }
                    else if (b != 'd' && b != 'i' && b != 'o' && b != 'u' && b != 'x' && b != 'X') { state = 0; again = true; }
                } else if (ch == 'h') flags |= 0x20; else if (ch == 'l') flags |= 0x10; else if (ch == 'w') flags |= 0x800;
                break;
            case 7: {
                bool numeric = false;
                int radix = 10;
                switch (ch) {
                case 'C': if (!(flags & 0x830)) flags |= 0x800; /* fallthrough */
                case 'c': {
                    uint32_t v = *arg(1);
                    static char cb[4];
                    if (!(flags & 0x810)) { cb[0] = (char)v; textlen = 1; }
                    else { textlen = h->wctomb(h->ctx, cb, (uint16_t)v); if (textlen < 0) no_out = 1; }
                    text = cb; wide = 0;
                    break;
                }
                case 'Z': {
                    const uint8_t* z = (const uint8_t*)h->ptr(h->ctx, *arg(1));  // {u16 len, u16 max, ptr buf}
                    const void* b = z ? h->ptr(h->ctx, *(const uint32_t*)(z + 4)) : nullptr;
                    if (!z || !b) { text = (flags & 0x800) ? (const void*)kNullW : (const void*)kNull; textlen = 6; wide = 0; }
                    else { textlen = *(const uint16_t*)z; text = b; wide = (flags & 0x800) != 0; if (wide) textlen /= 2; }
                    break;
                }
                case 'S': if (!(flags & 0x830)) flags |= 0x800; /* fallthrough */
                case 's': {
                    int lim = prec == -1 ? 0x7fffffff : prec;
                    const void* p = h->ptr(h->ctx, *arg(1));
                    if (!(flags & 0x810)) {
                        const char* s = (const char*)(p ? p : kNull); text = s; wide = 0;
                        int n = 0; while (lim-- > 0 && s[n]) ++n;
                        textlen = n;
                    } else {
                        const uint16_t* s = (const uint16_t*)(p ? p : kNullW); text = s; wide = 1;
                        int n = 0; while (lim-- > 0 && s[n]) ++n;
                        textlen = n;
                    }
                    break;
                }
                case 'E': case 'G': capexp = 1; ch += 0x20; /* fallthrough */
                case 'e': case 'f': case 'g': {
                    flags |= 0x40;
                    if (prec < 0) prec = 6; else if (prec == 0) { if (ch == 'g') prec = 1; } else if (prec > 0x200) prec = 0x200;
                    fbuf.assign(prec + 0x15d + 0x20, 0);
                    const uint32_t* d = arg(2);
                    h->cvt(h->ctx, d, fbuf.data(), ch, prec, capexp);
                    if ((flags & 0x80) && prec == 0) h->forcdecpt(h->ctx, fbuf.data());
                    if (ch == 'g' && !(flags & 0x80)) h->cropzeros(h->ctx, fbuf.data());
                    char* t = fbuf.data();
                    if (*t == '-') { flags |= 0x100; ++t; }
                    text = t; textlen = (int)strlen(t); wide = 0;
                    break;
                }
                case 'n': {
                    h->write_count(h->ctx, *arg(1), count, (flags & 0x20) != 0);
                    no_out = 1;
                    break;
                }
                case 'd': case 'i': flags |= 0x40; numeric = true; radix = 10; break;
                case 'u': numeric = true; radix = 10; break;
                case 'o': numeric = true; radix = 8; if (flags & 0x80) flags |= 0x200; break;
                case 'p': prec = 8; /* fallthrough */
                case 'X': hexadd = 7; numeric = true; radix = 16; if (flags & 0x80) { prefix[1] = (char)(hexadd + 'Q'); prefix[0] = '0'; prefixlen = 2; } break;
                case 'x': hexadd = 0x27; numeric = true; radix = 16; if (flags & 0x80) { prefix[1] = (char)(hexadd + 'Q'); prefix[0] = '0'; prefixlen = 2; } break;
                default: break;  // unknown type: the original emits whatever text state is stale
                }
                if (numeric) {
                    uint64_t v;
                    if (flags & 0x8000) { const uint32_t* a = arg(2); v = a[0] | ((uint64_t)a[1] << 32); }
                    else {
                        uint32_t a = *arg(1);
                        if (flags & 0x20) a = (flags & 0x40) ? (uint32_t)(int32_t)(int16_t)a : (uint16_t)a;
                        v = (flags & 0x40) ? (uint64_t)(int64_t)(int32_t)a : a;
                    }
                    if ((flags & 0x40) && (int64_t)v < 0) { v = (uint64_t)-(int64_t)v; flags |= 0x100; }
                    if (prec < 0) prec = 1; else { flags &= ~8u; if (prec > 0x200) prec = 0x200; }
                    if (!v) prefixlen = 0;
                    char* p = numbuf + sizeof numbuf - 1;
                    while (prec-- > 0 || v) {
                        int dg = (int)(v % radix); v /= radix;
                        dg += '0'; if (dg > '9') dg += hexadd;
                        *p-- = (char)dg;
                    }
                    textlen = (int)(numbuf + sizeof numbuf - 1 - p);
                    text = p + 1;
                    if ((flags & 0x200) && (*(const char*)text != '0' || textlen == 0)) { *p = '0'; --p; ++textlen; text = p + 1; }
                    wide = 0;
                }
                if (!no_out) {
                    if (flags & 0x40) {
                        if (flags & 0x100) prefix[0] = '-'; else if (flags & 1) prefix[0] = '+'; else if (flags & 2) prefix[0] = ' ';
                        if (prefix[0]) prefixlen = 1;  // ponytail: original keys on the flag chain, same result
                    }
                    int pad = width - prefixlen - textlen;
                    if (!(flags & 0xc)) multi(' ', pad);
                    str(prefix, prefixlen);
                    if ((flags & 8) && !(flags & 4)) multi('0', pad);
                    if (!wide || textlen < 1) str((const char*)text, textlen);
                    else {
                        const uint16_t* w = (const uint16_t*)text;
                        for (int i = 0; i < textlen; ++i) {
                            char o[4]; int n = h->wctomb(h->ctx, o, w[i]);
                            if (n < 1) break;
                            str(o, n);
                        }
                    }
                    if (flags & 4) multi(' ', pad);
                }
                break;
            }
            }
        }
        ch = *next; fmt = next;
    }
    return count;
}
