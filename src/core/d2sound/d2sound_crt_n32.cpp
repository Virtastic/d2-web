// MSVC 2003 CRT pieces of D2sound 1.13c. Win32 / heap / lock calls are host seams (function pointers in H);
// the stack-vs-malloc scratch choice of retail is replaced by the seam malloc (same results).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2sndHost {
    void* (*malloc)(size_t); void (*free)(void*);
    int (*MultiByteToWideChar)(unsigned cp, unsigned fl, const char* s, int n, uint16_t* w, int wn);
    int (*WideCharToMultiByte)(unsigned cp, unsigned fl, const uint16_t* w, int wn, char* s, int n, const char* def, int* used);
    int (*LCMapStringW)(unsigned lcid, unsigned fl, const uint16_t* s, int n, uint16_t* d, int dn);
    int (*LCMapStringA)(unsigned lcid, unsigned fl, const char* s, int n, char* d, int dn);
    int (*GetStringTypeW)(unsigned info, const uint16_t* s, int n, uint16_t* t);
    int (*GetStringTypeA)(unsigned lcid, unsigned info, const char* s, int n, uint16_t* t);
    int (*GetCPInfo)(unsigned cp, unsigned* maxCharSize);
    int (*GetLocaleInfoA)(unsigned lcid, unsigned type, char* buf, int n);
    unsigned (*GetLastError)();
    int wideMode;      // DAT_6f9c63a0 (GetStringTypeW support: 0 unknown, 1 yes, 2 no)
    int lcmapMode;     // DAT_6f9c63c8 (LCMapStringW support)
    unsigned defLcid;  // DAT_6f9c6388
    unsigned defCp;    // DAT_6f9c6398
};

static void* callocH(D2sndHost& h, size_t n, size_t sz) { void* p = h.malloc(n * sz); if (p) memset(p, 0, n * sz); return p; }

struct D2sndHeap { void* hHeap; int heapType; };  // DAT_6f9c6860 / DAT_6f9c6864
// 1.13c D2sound+0x02377
int D2sound_2377(D2sndHeap& g, int param, void* (*HeapCreate)(unsigned, size_t, size_t), void (*HeapDestroy)(void*),
                 int (*heap_select)(), int (*sbh_heap_init)(unsigned)) {
    g.hHeap = HeapCreate(param == 0, 0x1000, 0);
    if (!g.hHeap) return 0;
    g.heapType = heap_select();
    if (g.heapType == 3 && sbh_heap_init(0x3f8) == 0) { HeapDestroy(g.hHeap); return 0; }
    return 1;
}

struct D2sndSbh { void* regions; uint32_t cnt, used; void* cur; uint32_t thresh; uint32_t cap; };  // 6f9c661c,6614,6618,6624,6620,6628
// 1.13c D2sound+0x0351C
int D2sound_351C(D2sndSbh& s, void* (*HeapAlloc)(void*, unsigned, size_t), void* hHeap, uint32_t param) {
    s.regions = HeapAlloc(hHeap, 0, 0x140);
    if (!s.regions) return 0;
    s.cnt = 0; s.used = 0; s.cur = s.regions; s.thresh = param; s.cap = 0x10;
    return 1;
}

struct D2sndMbState {
    int userDefault;           // 6330
    int defCp;                 // 6398
    int curCp;                 // 6744
    uint8_t* cache;            // 6634 (0x220-byte block: +0 refcnt, +4 cp, +8 [6638], +0xc [6630], +0x10 u16[5], +0x1c u8[0x101], +0x11d u8[0x100])
    int v6638, v6630;
    uint16_t w6750[5];
    uint8_t t6640[0x101], t6760[0x100];
};
// 1.13c D2sound+0x033AE
int D2sound_33AE(D2sndMbState& m, int cp, D2sndHost& h, void (*lock)(int), void (*unlock)(),
                 unsigned (*GetOEMCP)(), unsigned (*GetACP)(), int (*setcp)(int)) {
    int r = -1;
    lock(0xd);
    m.userDefault = 0;
    if (cp == -2) { m.userDefault = 1; cp = (int)GetOEMCP(); }
    else if (cp == -3) { m.userDefault = 1; cp = (int)GetACP(); }
    else if (cp == -4) { m.userDefault = 1; cp = m.defCp; }
    if (cp == m.curCp) r = 0;
    else {
        uint8_t* blk = m.cache;
        if (!blk || *(int*)blk != 0) blk = (uint8_t*)h.malloc(0x220);
        if (blk && (r = setcp(cp)) == 0) {
            *(int*)blk = 0; *(int*)(blk + 4) = m.curCp; *(int*)(blk + 8) = m.v6638; *(int*)(blk + 0xc) = m.v6630;
            memcpy(blk + 0x10, m.w6750, 10);
            memcpy(blk + 0x1c, m.t6640, 0x101);
            m.cache = blk;
            memcpy(blk + 0x11d, m.t6760, 0x100);
        }
        if (r == -1 && blk != m.cache) h.free(blk);
    }
    unlock();
    return r;
}

// 1.13c D2sound+0x03A64 -- ___sbh_resize_block; mem = heap base, p1/blk/ offsets into it (32-bit retail pointers)
int D2sound_3A64(uint8_t* mem, uint32_t p1, uint32_t blk, int newSize) {
    auto W = [&](uint32_t a) -> uint32_t& { return *(uint32_t*)(mem + a); };
    auto B = [&](uint32_t a) -> uint8_t& { return mem[a]; };
    uint32_t u7 = W(p1 + 16), u10 = (blk - W(p1 + 12)) >> 15;
    uint32_t hdr = u10 * 0x204 + 0x144 + u7;
    uint32_t u12 = (uint32_t)(newSize + 0x17) & 0xfffffff0u;
    int iVar9 = (int)W(blk - 4) - 1;
    uint32_t p8 = W(blk - 4) - 5 + blk, u13 = W(p8);
    auto clamp = [](int v) -> uint32_t { return (uint32_t)(v > 0x3f ? 0x3f : v); };
    auto clearBit = [&](uint32_t i) {
        uint32_t& g = i < 0x20 ? W(p1) : W(p1 + 4);
        uint32_t mask = ~(0x80000000u >> ((i < 0x20 ? i : i - 0x20) & 31));
        W((i < 0x20 ? u7 + 0x44 : u7 + 0xc4) + u10 * 4) &= mask;
        if (--B(i + 4 + u7) == 0) g &= mask;
    };
    auto unlink = [&] { W(W(p8 + 8) + 4) = W(p8 + 4); W(W(p8 + 4) + 8) = W(p8 + 8); };
    auto insert = [&](uint32_t node, uint32_t i) {
        uint32_t b = hdr + i * 8;
        W(node + 4) = W(b + 4); W(node + 8) = b; W(b + 4) = node; W(W(node + 4) + 8) = node;
        if (W(node + 4) == W(node + 8)) {
            uint8_t c = B(i + 4 + u7)++;
            uint32_t pp;
            if (i < 0x20) { if (!c) W(p1) |= 0x80000000u >> (i & 31); pp = u7 + 0x44 + u10 * 4; }
            else { if (!c) W(p1 + 4) |= 0x80000000u >> ((i - 0x20) & 31); pp = u7 + 0xc4 + u10 * 4; i -= 0x20; }
            W(pp) |= 0x80000000u >> (i & 31);
        }
    };
    if (iVar9 < (int)u12) {
        if ((u13 & 1) || (int)(u13 + iVar9) < (int)u12) return 0;
        uint32_t lc = clamp(((int)u13 >> 4) - 1);
        if (W(p8 + 4) == W(p8 + 8)) clearBit(lc);
        unlink();
        int rem = (int)u13 + (iVar9 - (int)u12);
        if (rem > 0) {
            uint32_t node = blk - 4 + u12;
            insert(node, clamp((rem >> 4) - 1));
            W(node) = rem; W(rem - 4 + node) = rem;
        }
        W(blk - 4) = u12 + 1; W(blk - 8 + u12) = u12 + 1;
    } else if ((int)u12 < iVar9) {
        int sz = iVar9 - (int)u12;
        W(blk - 4) = u12 + 1;
        uint32_t node = blk - 4 + u12;
        W(node - 4) = u12 + 1;
        uint32_t u11 = clamp((sz >> 4) - 1);
        if (!(u13 & 1)) {
            if (W(p8 + 4) == W(p8 + 8)) clearBit(clamp(((int)u13 >> 4) - 1));
            unlink();
            sz += (int)u13;
            u11 = clamp((sz >> 4) - 1);
        }
        insert(node, u11);
        W(node) = sz; W(sz - 4 + node) = sz;
    }
    return 1;
}

// 1.13c D2sound+0x04E20 -- _strncmp (retail: -1 / 0 / 1 by unsigned byte compare)
int D2sound_4E20(const char* a, const char* b, size_t n) {
    if (!n) return 0;
    size_t len = 0;
    while (len < n && a[len]) len++;
    if (len < n) len++;  // include the NUL, as retail's scan count does
    for (size_t i = 0; i < len; i++) {
        uint8_t x = a[i], y = b[i];
        if (x != y) return y > x ? -1 : 1;
    }
    return 0;
}

// locale record fields read by the CRT ctype helpers (offsets in the retail struct)
struct D2sndLc {
    unsigned codepage;            // +0x04
    unsigned lcid;                // +0x14
    int clike;                    // +0x24
    int mbCurMax;                 // +0x28
    const uint16_t* pctype;       // +0x48
    int (*isctype)(int c, int mask);  // ___isctype_mt seam
};

// 1.13c D2sound+0x058A1 -- _atol (isspace via ctype table / isctype)
long D2sound_58A1(const char* s, const D2sndLc& lc) {
    auto sp = [&](uint8_t c) { return lc.mbCurMax < 2 ? (lc.pctype[c] & 8) != 0 : lc.isctype(c, 8) != 0; };
    while (sp((uint8_t)*s)) s++;
    uint32_t first = (uint8_t)*s;
    const uint8_t* p = (const uint8_t*)s + 1;
    uint32_t c = first;
    if (first == '-' || first == '+') { c = *p; p++; }
    int v = 0;
    while (c >= '0' && c <= '9') { v = (int)(c - '0') + v * 10; c = *p++; }
    return first == '-' ? -v : v;
}

// 1.13c D2sound+0x05929 -- code page of an LCID (LOCALE_IDEFAULTANSICODEPAGE 0x1004), -1 on failure
long D2sound_5929(unsigned lcid, D2sndHost& h, const D2sndLc& lc) {
    char buf[8] = {};
    if (!h.GetLocaleInfoA(lcid, 0x1004, buf, 6)) return -1;
    return D2sound_58A1(buf, lc);
}

// 1.13c D2sound+0x05B4A -- _isdigit
int D2sound_5B4A(int c, const D2sndLc& lc) {
    if (lc.mbCurMax > 1) return lc.isctype(c, 4);
    return lc.pctype[c] & 4;
}

// 1.13c D2sound+0x05970 -- code page to code page conversion; returns out, a malloc'd buffer, or null
char* D2sound_5970(D2sndHost& h, unsigned from, unsigned to, const char* src, uint32_t* pLen, char* out, int outLen) {
    char* res = nullptr;
    uint16_t* wide = nullptr;
    uint32_t cb = *pLen;
    if (from == to) return nullptr;
    bool sb = false;
    unsigned ci;
    if (h.GetCPInfo(from, &ci) && ci == 1 && h.GetCPInfo(to, &ci) && ci == 1) sb = true;
    uint32_t n;
    if (sb) n = cb == 0xffffffffu ? (uint32_t)strlen(src) + 1 : cb;
    else if (!(n = h.MultiByteToWideChar(from, 1, src, (int)cb, nullptr, 0))) return nullptr;
    wide = (uint16_t*)callocH(h, 2, n);
    if (!wide) return nullptr;
    if (h.MultiByteToWideChar(from, 1, src, (int)cb, wide, (int)n)) {
        if (!out) {
            if ((sb || (n = h.WideCharToMultiByte(to, 0, wide, (int)n, nullptr, 0, nullptr, nullptr))) &&
                (res = (char*)callocH(h, 1, n))) {
                uint32_t r = h.WideCharToMultiByte(to, 0, wide, (int)n, res, (int)n, nullptr, nullptr);
                if (!r) { h.free(res); res = nullptr; }
                else if (cb != 0xffffffffu) *pLen = r;
            }
        } else if (h.WideCharToMultiByte(to, 0, wide, (int)n, out, outLen, nullptr, nullptr)) res = out;
    }
    h.free(wide);
    return res;
}

// 1.13c D2sound+0x04EA0 -- LCMapStringA (wide path when the host has LCMapStringW, else via the ANSI code page)
unsigned D2sound_4EA0(D2sndHost& h, unsigned lcid, unsigned fl, const char* src, uint32_t len, char* out, int outLen,
                      unsigned cp, int bErr, const D2sndLc& lc) {
    if (!h.lcmapMode) {
        if (!h.LCMapStringW(0, 0x100, (const uint16_t*)u"", 1, nullptr, 0)) { if (h.GetLastError() == 0x78) h.lcmapMode = 2; }
        else h.lcmapMode = 1;
    }
    if (len > 0 && (int)len > 0) {  // clamp to the first NUL (inclusive)
        uint32_t i = 0;
        while (i < len && src[i]) i++;
        if (i < len) len = i + 1;
    }
    if (h.lcmapMode != 2 && h.lcmapMode != 0) {
        if (h.lcmapMode != 1) return 0;
        unsigned r = 0;
        if (!cp) cp = h.defCp;
        int n = h.MultiByteToWideChar(cp, bErr ? 9 : 1, src, (int)len, nullptr, 0);
        if (!n) return 0;
        uint16_t* w = (uint16_t*)h.malloc((size_t)n * 2);
        if (!w) return 0;
        uint16_t* w2 = nullptr;
        if (h.MultiByteToWideChar(cp, 1, src, (int)len, w, n)) {
            r = h.LCMapStringW(lcid, fl, w, n, nullptr, 0);
            if (r) {
                if (!(fl & 0x400)) {
                    w2 = (uint16_t*)h.malloc((size_t)r * 2);
                    if (w2 && h.LCMapStringW(lcid, fl, w, n, w2, (int)r))
                        r = h.WideCharToMultiByte(cp, 0, w2, (int)r, outLen ? out : nullptr, outLen, nullptr, nullptr);
                } else if (outLen && (int)r <= outLen) h.LCMapStringW(lcid, fl, w, n, (uint16_t*)out, outLen);
            }
        }
        if (w2) h.free(w2);
        h.free(w);
        return r;
    }
    if (!lcid) lcid = h.defLcid;
    if (!cp) cp = h.defCp;
    long acp = D2sound_5929(lcid, h, lc);
    if (acp == -1) return 0;
    if ((unsigned)acp == cp) return h.LCMapStringA(lcid, fl, src, (int)len, out, outLen);
    uint32_t l = len;
    char* conv = D2sound_5970(h, cp, (unsigned)acp, src, &l, nullptr, 0);
    if (!conv) return 0;
    unsigned r = 0;
    char* back = nullptr;
    uint32_t m = h.LCMapStringA(lcid, fl, conv, (int)l, nullptr, 0);
    if (m) {
        char* buf = (char*)callocH(h, 1, m);
        if (buf) {
            m = h.LCMapStringA(lcid, fl, conv, (int)l, buf, (int)m);
            if (m) {
                uint32_t k = m;
                back = D2sound_5970(h, (unsigned)acp, cp, buf, &k, out, outLen);
                r = back ? k : 0;
            }
            h.free(buf);
        }
    }
    h.free(conv);
    if (back && back != out) h.free(back);
    return r;
}

// 1.13c D2sound+0x04C58 -- ___crtGetStringTypeA (args in retail cdecl order)
int D2sound_4C58(D2sndHost& h, unsigned info, const char* src, int cch, uint16_t* out, unsigned cp, unsigned lcid, int bErr,
                 const D2sndLc& lc) {
    if (!h.wideMode) {
        uint16_t t[2];
        if (!h.GetStringTypeW(1, (const uint16_t*)u"", 1, t)) { if (h.GetLastError() == 0x78) h.wideMode = 2; }
        else h.wideMode = 1;
    }
    if (h.wideMode == 2 || h.wideMode == 0) {
        if (!lcid) lcid = h.defLcid;
        unsigned dcp = cp ? cp : h.defCp;
        long acp = D2sound_5929(lcid, h, lc);
        if (acp == -1) return 0;
        char* conv = nullptr;
        uint32_t l = (uint32_t)cch;
        if ((unsigned)acp != dcp) {
            conv = D2sound_5970(h, dcp, (unsigned)acp, src, &l, nullptr, 0);
            if (!conv) return 0;
            src = conv;
        }
        int r = h.GetStringTypeA(lcid, info, src, (int)l, out);
        if (conv) h.free(conv);
        return r;
    }
    if (h.wideMode != 1) return 0;
    if (!cp) cp = h.defCp;
    int n = h.MultiByteToWideChar(cp, bErr ? 9 : 1, src, cch, nullptr, 0);
    if (!n) return 0;
    uint16_t* w = (uint16_t*)callocH(h, 2, n);
    if (!w) return 0;
    int r = 0;
    if (h.MultiByteToWideChar(cp, 1, src, cch, w, n)) r = h.GetStringTypeW(info, w, n, out);
    h.free(w);
    return r;
}

// 1.13c D2sound+0x05B84 -- ___tolower_mt
unsigned D2sound_5B84(D2sndHost& h, const D2sndLc& lc, unsigned c) {
    if (lc.lcid == 0 || (lc.clike != 0 && c < 0x80)) {
        if ((int)c > 0x40 && (int)c < 0x5b) return c + 0x20;
        return c;
    }
    if (c < 0x100) {
        unsigned up = lc.mbCurMax < 2 ? (lc.pctype[c] & 1) : (unsigned)lc.isctype((int)c, 1);
        if (!up) return c;
    }
    char in[4] = {};
    unsigned n;
    if (!(lc.pctype[(c >> 8) & 0xff] & 0x8000)) { in[0] = (char)c; n = 1; }
    else { in[0] = (char)(c >> 8); in[1] = (char)c; n = 2; }
    uint8_t o[4] = {};
    unsigned r = D2sound_4EA0(h, lc.lcid, 0x100, in, n, (char*)o, 3, lc.codepage, 1, lc);
    if (!r) return c;
    return r == 1 ? o[0] : (unsigned)(o[0] << 8 | o[1]);
}

struct D2sndMbi { uintptr_t base, allocBase, region; unsigned state, protect; };
struct D2sndVm {
    size_t (*VirtualQuery)(uintptr_t addr, D2sndMbi*);
    unsigned (*PageSize)();
    int (*VirtualAlloc)(uintptr_t addr, size_t n, unsigned type, unsigned prot);
    int (*VirtualProtect)(uintptr_t addr, size_t n, unsigned prot, unsigned* old);
    int platform;  // DAT_6f9c6004 (1 = Win9x)
};
// 1.13c D2sound+0x057BE -- __resetstkoflw (Win32 virtual-memory calls are seams)
int D2sound_57BE(D2sndVm& vm, uintptr_t sp) {
    D2sndMbi mbi;
    if (!vm.VirtualQuery(sp, &mbi)) return 0;
    uintptr_t page = vm.PageSize();
    uintptr_t cur = (sp & ~(page - 1)) - page;
    uintptr_t floor_ = (vm.platform != 1 ? 2 : 0x11) * page + mbi.allocBase;
    if (floor_ > cur) return 0;
    uintptr_t at = cur;
    if (vm.platform != 1) {
        at = mbi.allocBase;
        do {
            if (!vm.VirtualQuery(at, &mbi)) return 0;
            at += mbi.region;
        } while (!(mbi.state & 0x1000));
        at = mbi.base;
        if ((mbi.protect >> 8) & 1) return 1;
        if (cur < mbi.base) return 0;
        if (mbi.base < floor_) at = floor_;
        vm.VirtualAlloc(at, page, 0x1000, 4);
    }
    unsigned old;
    return vm.VirtualProtect(at, page, (vm.platform != 1 ? 0x103 : 0) + 1, &old);
}
