// src/core/storm/storm_n72.cpp -- Storm batch n72 (12 lowest UNMAPPED rvas). Host seams are
// function pointers; intrusive links are pointer-width (see storm_sub_2780.cpp convention).
#include <cstdint>
#include <cstring>
#include <string>

extern void StormSub2780ListUnlinkAll(struct S2780List* list);

struct S2780Link { uintptr_t prevlink, next; };
struct S2780List { intptr_t linkoffset; S2780Link term; };

// ---------------------------------------------------------------- 0x2DD0 hash-table grow
struct S2DD0Table { S2780List* buckets; uint32_t mask; };  // retail +0x1c, +0x24
struct S2DD0Env { void (*growBuckets)(S2DD0Table* t, uint32_t newCount); };  // Storm+0x32E0

static void n72_unlinkLink(S2780Link* l) {  // shared splice: l is a link at address l
    uintptr_t prev = l->prevlink;
    if (!prev) return;
    uintptr_t next = l->next;
    uintptr_t* target = (intptr_t)next < 0 ? (uintptr_t*)~next
        : (uintptr_t*)((uintptr_t)l + (next - ((S2780Link*)prev)->next));
    *target = prev;
    ((S2780Link*)prev)->next = next;
    l->prevlink = 0; l->next = 0;
}

// 1.13c STORM+0x02DD0
extern "C" void Storm_Sub2DD0(const S2DD0Env& e, S2DD0Table* t) {
    uint32_t oldN = t->mask + 1, newN = oldN * 2;
    S2780Link tmp; tmp.prevlink = (uintptr_t)&tmp; tmp.next = ~(uintptr_t)&tmp;  // stack list head (local_14/local_10)
    for (uint32_t i = 0; i < oldN; i++) {
        S2780List* b = &t->buckets[i];
        while ((intptr_t)b->term.next > 0) {
            uintptr_t n = b->term.next;
            S2780Link* l = (S2780Link*)(n + sizeof(uintptr_t));  // link offset 4 (pointer-width here)
            n72_unlinkLink(l);
            S2780Link* tail = (S2780Link*)tmp.prevlink;
            l->prevlink = (uintptr_t)tail;
            l->next = tail->next;
            tail->next = n;
            tmp.prevlink = (uintptr_t)l;
        }
    }
    e.growBuckets(t, newN);
    for (uint32_t i = 0; i < newN; i++) {
        S2780List* b = &t->buckets[i];
        if (b->linkoffset != (intptr_t)sizeof(uintptr_t)) {
            StormSub2780ListUnlinkAll(b);
            b->linkoffset = sizeof(uintptr_t);
            b->term.prevlink = (uintptr_t)&b->term;
            b->term.next = ~(uintptr_t)&b->term;
        }
    }
    t->mask = newN - 1;
    while ((intptr_t)tmp.next > 0) {
        uintptr_t n = tmp.next;
        S2780List* b = &t->buckets[*(uint32_t*)n & t->mask];
        S2780Link* l = (S2780Link*)(n + b->linkoffset);
        n72_unlinkLink(l);
        S2780Link* tail = (S2780Link*)b->term.prevlink;
        l->prevlink = (uintptr_t)tail;
        l->next = tail->next;
        tail->next = n;
        b->term.prevlink = (uintptr_t)l;
    }
    // retail tail: temp-list destructor (Storm+0x2780 on the empty stack list, then link self-reset)
    if (tmp.prevlink) {
        uintptr_t* tg = (intptr_t)tmp.next < 0 ? (uintptr_t*)~tmp.next : (uintptr_t*)tmp.next;
        *tg = tmp.prevlink;
        ((S2780Link*)tmp.prevlink)->next = tmp.next;
    }
}

// ---------------------------------------------------------------- 0x46D0 destructor
struct S46D0Env {
    void (*elemDtor)(uint8_t* e);    // LAB_6fbf44c0
    void (*baseDtor)(uint8_t* self); // Storm+0x4B80
};
// 1.13c STORM+0x046D0
extern "C" void Storm_Sub46D0(const S46D0Env& e, uint8_t* self) {
    for (int i = 2; i-- > 0;) e.elemDtor(self + 0x24 * 4 + i * 4);  // _eh_vector_destructor_iterator_(+0x90, 4, 2)
    e.baseDtor(self);
}

// ---------------------------------------------------------------- Huffman node/tree (0x4810, 0x4980)
struct HuffNode {  // retail words: [0]prev [1]next [2]sym [3]weight [4]parent [5]child
    uintptr_t prev, next;
    uint32_t sym, weight;
    HuffNode* parent;
    HuffNode* child;
};
struct HuffTree {
    uint32_t* swaps;      // retail tree+4
    HuffNode* term;       // retail tree+0x3060: list terminator link
};

static void n72_unlinkNode(HuffNode* L) { n72_unlinkLink((S2780Link*)L); }

// 1.13c STORM+0x04810
extern "C" void Storm_Sub4810(HuffTree* tree, HuffNode* a) {
    while (a) {
        uint32_t w = a->weight + 1;
        a->weight = w;
        HuffNode* p3; HuffNode* p4 = a;
        for (;;) {
            p3 = p4;
            p4 = (HuffNode*)p3->next;
            if (!((intptr_t)p4 > 0)) { p4 = nullptr; break; }
            if (w <= p4->weight) break;
        }
        if (p3 != a) {
            n72_unlinkNode(p3);
            HuffNode* ap = (HuffNode*)a->prev;
            p3->prev = (uintptr_t)ap;
            p3->next = ap->next;
            ap->next = (uintptr_t)p3;
            a->prev = (uintptr_t)p3;
            // unlink a (its prev is now p3)
            {
                uintptr_t next = a->next;
                uintptr_t* t = (intptr_t)next < 0 ? (uintptr_t*)~next
                    : (uintptr_t*)((uintptr_t)a + (next - p3->next));
                *t = (uintptr_t)p3;
                ((HuffNode*)a->prev)->next = a->next;
                a->prev = 0; a->next = 0;
            }
            HuffNode* at = p4 ? p4 : tree->term;
            HuffNode* pp = (HuffNode*)at->prev;
            a->prev = (uintptr_t)pp;
            a->next = pp->next;
            pp->next = (uintptr_t)a;
            at->prev = (uintptr_t)a;
            HuffNode* oldSibChild = p3->parent->child;
            if (a->parent->child == a) a->parent->child = p3;
            if (oldSibChild == p3) p3->parent->child = a;
            HuffNode* tp = a->parent; a->parent = p3->parent; p3->parent = tp;
            ++*tree->swaps;
        }
        a = a->parent;
    }
}

struct HuffBits { const uint8_t* p; uint32_t bits, cnt; };  // EAX reader: ptr, bit buffer, bit count
struct HuffCache { uint32_t stamp, len; uintptr_t val; };    // tree+0x3474, 128 entries

// 1.13c STORM+0x04980
extern "C" uint32_t Storm_Sub4980(HuffTree* tree, HuffCache* cache, HuffBits* r) {
    if (r->cnt < 8) {
        uint16_t w; memcpy(&w, r->p, 2);
        r->bits |= (uint32_t)w << (r->cnt & 31);
        r->cnt += 16; r->p += 2;
    }
    uint32_t bits = r->bits;
    HuffCache* ent = &cache[bits & 0x7f];
    bool miss = ent->stamp != *tree->swaps;
    uintptr_t node;
    if (miss) {
        HuffNode* last = (HuffNode*)tree->term->prev;
        uintptr_t v = ((HuffNode*)last->prev)->next;
        node = ((intptr_t)v < 1 ? 0 : v);
    } else {
        node = ent->val;
        if (ent->len < 8) {
            r->bits = bits >> (ent->len & 31);
            r->cnt -= ent->len;
            return (uint32_t)ent->val;
        }
        r->bits = bits >> 7;
        r->cnt -= 7;
    }
    uint32_t depth = 0; uintptr_t at7 = 0;
    do {
        uint32_t cur = r->bits;
        node = (uintptr_t)((HuffNode*)node)->child;
        r->bits = cur >> 1;
        uint32_t c = r->cnt - 1;
        r->cnt = c;
        if (c == 0) { memcpy(&r->bits, r->p, 4); r->p += 4; r->cnt = 32; }
        if (cur & 1) node = ((HuffNode*)node)->next;
        depth++;
        if (depth == 7) at7 = node;
    } while (((HuffNode*)node)->child != nullptr);
    uint32_t sym = ((HuffNode*)node)->sym;
    if (miss) {
        if (depth > 7) {
            ent->len = depth; ent->stamp = *tree->swaps; ent->val = at7;
            return sym;
        }
        uint32_t step = 1u << (depth & 31);
        uint32_t idx = (0xffffffffu >> ((0x20 - depth) & 31)) & bits & 0x7f;
        do {
            cache[idx].stamp = *tree->swaps; cache[idx].len = depth; cache[idx].val = sym;
            idx += step;
        } while (idx < 0x80);
    }
    return sym;
}

// ---------------------------------------------------------------- 0x687E CRT wcstombs core
struct S687ELocale { uint32_t codepage; uint32_t nonC; };  // retail +4, +0x14
struct S687EEnv {
    uint32_t mbCurMax;  // DAT_6fc406b0
    int (*wideToMulti)(uint32_t cp, const uint16_t* w, int cw, char* out, int cb, int* usedDefault);  // WideCharToMultiByte(cp,0,..)
    uint32_t (*getLastError)();
    uint32_t (*wcsLenC)(const uint16_t* w);  // Storm+0x6868
    int32_t* (*errnoPtr)();                  // Storm+0xA062
};
// 1.13c STORM+0x0687E
extern "C" uint32_t Storm_Sub687E(const S687EEnv& e, const S687ELocale* loc, char* dst, const uint16_t* src, uint32_t n) {
    uint32_t r = 0; const uint16_t* wp = src; int def = 0;
    if (!dst) {
        if (!loc->nonC) return e.wcsLenC(src);
        r = e.wideToMulti(loc->codepage, src, -1, nullptr, 0, &def);
        if (r != 0 && def == 0) return r - 1;
    } else {
        if (n == 0) return 0;
        if (!loc->nonC) {
            while (*src < 0x100) {
                dst[r] = (char)*src;
                uint16_t ch = *src++;
                if (ch == 0) return r;
                r++;
                if (n <= r) return r;
            }
        } else if (e.mbCurMax == 1) {
            const uint16_t* p = src; uint32_t k = n;
            if (n != 0) {
                do { if (*p == 0) break; p++; k--; } while (k != 0);
                if (k != 0 && *p == 0) n = (uint32_t)(p - src) + 1;
            }
            r = e.wideToMulti(loc->codepage, src, (int)n, dst, (int)n, &def);
            if (r != 0 && def == 0) {
                if (dst[r - 1] != 0) return r;
                return r - 1;
            }
        } else {
            int rc = e.wideToMulti(loc->codepage, src, -1, dst, (int)n, &def);
            if (rc == 0) {
                if (def == 0) {
                    r = 0;
                    if (e.getLastError() == 0x7a) {
                        for (;;) {
                            if (n <= r) return r;
                            char tmp[8];
                            int k = e.wideToMulti(loc->codepage, wp, 1, tmp, (int)e.mbCurMax, &def);
                            if (k == 0 || def != 0) break;
                            if (n < (uint32_t)k + r) return r;
                            for (int i = 0; i < k; i++) {
                                char c = tmp[i];
                                dst[r] = c;
                                if (c == 0) return r;
                                r++;
                            }
                            wp++;
                        }
                    }
                }
            } else if (def == 0) return (uint32_t)rc - 1;
        }
    }
    *e.errnoPtr() = 0x2a;
    return 0xffffffffu;
}

// ---------------------------------------------------------------- 0xA059 / 0xA062 / 0xA06B / 0xA15E
// 1.13c STORM+0x0A059
extern "C" void Storm_SubA059(void (*sub902A)(int)) { sub902A(7); }

struct SPtd { uint8_t* (*getptd)(); };  // Storm+0x76FA
// 1.13c STORM+0x0A062
extern "C" int32_t* Storm_SubA062(const SPtd& e) { return (int32_t*)(e.getptd() + 8); }   // _tiddata._terrno
// 1.13c STORM+0x0A06B
extern "C" uint32_t* Storm_SubA06B(const SPtd& e) { return (uint32_t*)(e.getptd() + 0xC); }  // _tiddata._tdoserrno

// 1.13c STORM+0x0A15E
extern "C" void Storm_ReportFailureA15E(void (*sub_dd5f)(int), void (*exitProcess)(uint32_t)) {
    sub_dd5f(1);
    exitProcess(3);
}

// ---------------------------------------------------------------- 0xD703 / 0xD74A
struct SD703Env {
    int (*getLocaleInfoA)(uint32_t lcid, uint32_t type, char* buf, int cch);
};
// 1.13c STORM+0x0D703
extern "C" int32_t Storm_SubD703(const SD703Env& e, uint32_t lcid) {
    char buf[7] = {0};
    if (e.getLocaleInfoA(lcid, 0x1004, buf, 6) == 0) return -1;  // LOCALE_IDEFAULTANSICODEPAGE
    int32_t v = 0; const char* p = buf; bool neg = false;
    while (*p == ' ' || (*p >= 9 && *p <= 13)) p++;
    if (*p == '-') { neg = true; p++; } else if (*p == '+') p++;
    while (*p >= '0' && *p <= '9') v = v * 10 + (*p++ - '0');
    return neg ? -v : v;
}

struct SD74AEnv {
    int (*getCPMaxChar)(uint32_t cp);  // GetCPInfo; returns -1 on failure
    int (*multiToWide)(uint32_t cp, const char* s, int cb, uint16_t* w, int cw);   // flags MB_PRECOMPOSED
    int (*wideToMulti)(uint32_t cp, const uint16_t* w, int cw, char* out, int cb); // flags 0
    void* (*allocZ)(uint32_t elemSize, uint32_t count);  // Storm+0x9198
    void (*free)(void*);
};
// 1.13c STORM+0x0D74A
extern "C" char* Storm_SubD74A(const SD74AEnv& e, uint32_t cpFrom, uint32_t cpTo, const char* src,
                               uint32_t* pCb, char* dst, int dstSize) {
    char* result = nullptr;
    uint32_t cb = *pCb;
    if (cpFrom == cpTo) return nullptr;
    bool single = e.getCPMaxChar(cpFrom) == 1 && e.getCPMaxChar(cpTo) == 1;
    uint32_t wlen = 0;
    if (single) wlen = (cb == 0xffffffffu) ? (uint32_t)strlen(src) + 1 : cb;
    else {
        wlen = (uint32_t)e.multiToWide(cpFrom, src, (int)cb, nullptr, 0);
        if (wlen == 0) return nullptr;
    }
    uint16_t* w = (uint16_t*)e.allocZ(2, wlen);
    if (!w) return nullptr;
    if (e.multiToWide(cpFrom, src, (int)cb, w, (int)wlen) != 0) {
        if (!dst) {
            if (single || (wlen = (uint32_t)e.wideToMulti(cpTo, w, (int)wlen, nullptr, 0)) != 0) {
                char* out = (char*)e.allocZ(1, wlen);
                if (out) {
                    uint32_t u = (uint32_t)e.wideToMulti(cpTo, w, (int)wlen, out, (int)wlen);
                    if (u == 0) e.free(out);
                    else { result = out; if (cb != 0xffffffffu) *pCb = u; }
                }
            }
        } else if (e.wideToMulti(cpTo, w, (int)wlen, dst, dstSize) != 0) result = dst;
    }
    e.free(w);
    return result;
}

// ---------------------------------------------------------------- 0x7B77 CRT _output (printf engine)
struct S7B77Env {
    int32_t count;                                              // local_228 (chars written, -1 on error)
    void (*writeChar)(S7B77Env*, int ch);                       // Storm+0x7AE9
    void (*writeStr)(S7B77Env*, const uint8_t* p, int n);       // Storm+0x7B40
    void (*writeMulti)(S7B77Env*, int ch, int n);               // Storm+0x7B1C
    bool (*isLead)(uint32_t c);                                 // _pctype[c] & 0x80
    int (*wctomb)(char* out, uint16_t wc);
    void* (*malloc)(size_t);
    void (*free)(void*);
    void (*fltcvt)(const uint8_t dbl[8], char* buf, int fmt, int prec, int caps);  // __fptrap[0]
    void (*cropzeros)(char* buf);                               // __fptrap[1]
    void (*forcdecpt)(char* buf);                               // __fptrap[3]
    uint8_t (*rd8)(uint32_t a);  uint16_t (*rd16)(uint32_t a);  uint32_t (*rd32)(uint32_t a);
    void (*wr16)(uint32_t a, uint16_t v);  void (*wr32)(uint32_t a, uint32_t v);
};

static const uint8_t k7B77Tab[0x59] = {
 0x06,0x00,0x00,0x06,0x00,0x01,0x00,0x00,0x10,0x00,0x03,0x06,0x00,0x06,0x02,0x10,0x04,0x45,0x45,0x45,0x05,0x05,0x05,0x05,0x05,0x35,0x30,0x00,0x50,0x00,0x00,0x00,0x00,0x20,0x28,0x38,0x50,0x58,0x07,0x08,0x00,0x37,0x30,0x30,0x57,0x50,0x07,0x00,0x00,0x20,0x20,0x08,0x00,0x00,0x00,0x00,0x08,0x60,0x68,0x60,0x60,0x60,0x60,0x00,0x00,0x70,0x70,0x78,0x78,0x78,0x78,0x08,0x07,0x08,0x00,0x00,0x07,0x00,0x08,0x08,0x08,0x00,0x00,0x08,0x00,0x08,0x00,0x07,0x08};

// 1.13c STORM+0x07B77
extern "C" int32_t Storm_Output7B77(S7B77Env& e, const uint8_t* fmt, const uint8_t* ap) {
    auto slot32 = [&](const uint8_t* a) { uint32_t v; memcpy(&v, a, 4); return v; };
    uint8_t c = *fmt;
    int st = 0;
    e.count = 0;
    uint32_t flags = 0;
    int width = 0, prec = -1, prefLen = 0, hexadd = 0, caps = 0;
    char pref[2] = {0, 0};
    bool wide = false, noOut = false;
    int len = 0;
    uint32_t radix = 0;
    const uint8_t* text = nullptr;
    std::u16string wbuf; std::string sbuf;
    char lbuf[0x240];
    void* heap = nullptr;
    const uint8_t* next;
    for (;;) {
        if (c == 0 || (next = fmt + 1, e.count < 0)) return e.count;
        uint32_t cls = (c < 0x20 || c > 'x') ? 0 : (k7B77Tab[c - 0x20] & 0xf);
        st = (int8_t)k7B77Tab[cls * 8 + st] >> 4;
        const uint8_t* nap = ap;
        auto literal = [&]() {
            wide = false;
            if (e.isLead(c)) { e.writeChar(&e, c); next = fmt + 2; c = fmt[1]; }
            e.writeChar(&e, c);
        };
        switch (st) {
        case 0: literal(); break;
        case 1: prec = -1; caps = 0; noOut = false; width = 0; prefLen = 0; flags = 0; wide = false; break;
        case 2:
            if (c == 0x20) flags |= 2; else if (c == 0x23) flags |= 0x80; else if (c == 0x2b) flags |= 1;
            else if (c == 0x2d) flags |= 4; else if (c == 0x30) flags |= 8;
            break;
        case 3:
            if (c == '*') {
                width = (int32_t)slot32(ap); nap = ap + 4;
                if (width < 0) { flags |= 4; width = -width; }
            } else width = (char)c - '0' + width * 10;
            break;
        case 4: prec = 0; break;
        case 5:
            if (c == '*') { prec = (int32_t)slot32(ap); nap = ap + 4; if (prec < 0) prec = -1; }
            else prec = (char)c - '0' + prec * 10;
            break;
        case 6:
            if (c == 0x49) {
                uint8_t b1 = fmt[1];
                if (b1 == '6' && fmt[2] == '4') { next = fmt + 3; flags |= 0x8000; }
                else if (b1 == '3' && fmt[2] == '2') { next = fmt + 3; flags &= 0xffff7fffu; }
                else if (b1 != 'd' && b1 != 'i' && b1 != 'o' && b1 != 'u' && b1 != 'x' && b1 != 'X') {
                    st = 0; literal();
                }
            } else if (c == 0x68) flags |= 0x20;
            else if (c == 0x6c) flags |= 0x10;
            else if (c == 0x77) flags |= 0x800;
            break;
        case 7: {
            enum { NONE, INT, FLT, TXT } kind = NONE;   // NONE: default/falls to output with stale state
            bool doOut = true;
            if (c == 'n') {
                nap = ap + 4;
                if (!(flags & 0x20)) e.wr32(slot32(ap), (uint32_t)e.count);
                else e.wr16(slot32(ap), (uint16_t)e.count);
                noOut = true; doOut = false;
            } else if (c == 'C' || c == 'c') {
                if (c == 'C' && (flags & 0x830) == 0) flags |= 0x800;
                nap = ap + 4;
                uint16_t wc; memcpy(&wc, ap, 2);
                if ((flags & 0x810) == 0) { lbuf[0] = (char)wc; len = 1; }
                else { len = e.wctomb(lbuf, wc); if (len < 0) noOut = true; }
                text = (const uint8_t*)lbuf;
            } else if (c == 'Z') {
                nap = ap + 8 - 4;
                uint32_t sp = slot32(ap), buf = sp ? e.rd32(sp + 4) : 0;
                if (!sp || !buf) { sbuf = "(null)"; text = (const uint8_t*)sbuf.c_str(); len = 6; }
                else {
                    len = e.rd16(sp);
                    if (!(flags & 0x800)) { wide = false; sbuf.clear(); for (int i = 0; i < len; i++) sbuf += (char)e.rd8(buf + i); text = (const uint8_t*)sbuf.data(); }
                    else { len /= 2; wide = true; wbuf.clear(); for (int i = 0; i < len; i++) wbuf += (char16_t)e.rd16(buf + i * 2); }
                }
            } else if (c == 's' || c == 'S') {
                if (c == 'S' && (flags & 0x830) == 0) flags |= 0x800;
                int lim = prec == -1 ? 0x7fffffff : prec;
                nap = ap + 4;
                uint32_t a = slot32(ap);
                int n = 0;
                if ((flags & 0x810) == 0) {
                    if (!a) { sbuf = "(null)"; n = 0; for (const char* q = sbuf.c_str(); lim != 0 && (lim--, q[n] != 0); n++) {} }
                    else { sbuf.clear(); while (lim != 0 && (lim--, e.rd8(a + n) != 0)) { sbuf += (char)e.rd8(a + n); n++; } }
                    text = (const uint8_t*)sbuf.c_str();
                } else {
                    wide = true; wbuf.clear();
                    if (!a) { for (const char16_t* q = u"(null)"; lim != 0 && (lim--, q[n] != 0); n++) wbuf += q[n]; }
                    else while (lim != 0 && (lim--, e.rd16(a + n * 2) != 0)) { wbuf += (char16_t)e.rd16(a + n * 2); n++; }
                }
                len = n;
            } else if (c == 'E' || c == 'G' || (c >= 'e' && c <= 'g')) {
                if (c == 'E' || c == 'G') { caps = 1; c += 0x20; }
                flags |= 0x40;
                char* buf = lbuf;
                if (prec < 0) prec = 6;
                else if (prec == 0) { if (c == 'g') prec = 1; }
                else {
                    if (prec > 0x200) prec = 0x200;
                    if (prec > 0xa3) {
                        heap = e.malloc(prec + 0x15d);
                        if (heap) buf = (char*)heap; else prec = 0xa3;
                    }
                }
                nap = ap + 8;
                e.fltcvt(ap, buf, (int)(char)c, prec, caps);
                if ((flags & 0x80) && prec == 0) e.forcdecpt(buf);
                if (c == 'g' && !(flags & 0x80)) e.cropzeros(buf);
                char* t = buf;
                if (*t == '-') { flags |= 0x100; t++; }
                text = (const uint8_t*)t;
                len = (int)strlen(t);
            } else if (c == 'd' || c == 'i' || c == 'o' || c == 'u' || c == 'x' || c == 'X' || c == 'p') {
                kind = INT;
                if (c == 'd' || c == 'i') { flags |= 0x40; radix = 10; }
                else if (c == 'u') radix = 10;
                else if (c == 'o') { radix = 8; if (flags & 0x80) flags |= 0x200; }
                else {
                    if (c == 'p') prec = 8;
                    hexadd = (c == 'x') ? 0x27 : 7;
                    radix = 16;
                    if (flags & 0x80) { pref[1] = (char)(hexadd + 'Q'); pref[0] = '0'; prefLen = 2; }
                }
                uint32_t lo, hi;
                if (!(flags & 0x8000)) {
                    nap = ap + 4;
                    lo = slot32(ap);
                    if (!(flags & 0x20)) hi = (flags & 0x40) ? (uint32_t)((int32_t)lo >> 31) : 0;
                    else {
                        lo = (flags & 0x40) ? (uint32_t)(int32_t)(int16_t)lo : (uint32_t)(uint16_t)lo;
                        hi = (uint32_t)((int32_t)lo >> 31);
                    }
                } else { lo = slot32(ap); hi = slot32(ap + 4); nap = ap + 8; }
                uint64_t v = ((uint64_t)hi << 32) | lo;
                if ((flags & 0x40) && (int64_t)v < 0) { v = (uint64_t)-(int64_t)v; flags |= 0x100; }
                if (!(flags & 0x8000)) v &= 0xffffffffu;
                if (prec < 0) prec = 1;
                else { flags &= ~8u; if (prec > 0x200) prec = 0x200; }
                if (v == 0) prefLen = 0;
                char* q = lbuf + sizeof(lbuf) - 1;
                while (prec > 0 || v != 0) {
                    prec--;
                    int d = (int)(v % radix) + '0';
                    v /= radix;
                    if (d > '9') d += hexadd;
                    *q-- = (char)d;
                }
                len = (int)(lbuf + sizeof(lbuf) - 1 - q);
                text = (const uint8_t*)(q + 1);
                if ((flags & 0x200) && (*text != '0' || len == 0)) { *q = '0'; len++; text = (const uint8_t*)q; }
            }
            (void)kind;
            if (doOut && !noOut) {
                if (flags & 0x40) {
                    if (flags & 0x100) pref[0] = '-'; else if (flags & 1) pref[0] = '+'; else if (flags & 2) pref[0] = ' ';
                    if ((flags & 0x100) || (flags & 3)) prefLen = 1;
                }
                int pad = width - prefLen - len;
                if (!(flags & 0xc)) e.writeMulti(&e, 0x20, pad);
                e.writeStr(&e, (const uint8_t*)pref, prefLen);
                if ((flags & 8) && !(flags & 4)) e.writeMulti(&e, '0', pad);
                if (!wide || len < 1) { if (len > 0 || text) e.writeStr(&e, text, len); }
                else {
                    for (int i = 0, left = len; left > 0; i++, left--) {
                        char tmp[8];
                        int n = e.wctomb(tmp, (uint16_t)wbuf[i]);
                        if (n < 1) break;
                        e.writeStr(&e, (const uint8_t*)tmp, n);
                    }
                }
                if (flags & 4) e.writeMulti(&e, 0x20, pad);
            }
            if (heap) { e.free(heap); heap = nullptr; }
            break;
        }
        }
        c = *next;
        fmt = next;
        ap = nap;
    }
}
