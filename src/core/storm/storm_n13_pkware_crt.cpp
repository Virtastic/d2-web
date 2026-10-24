// src/core/storm/storm_n13_pkware_crt.cpp -- 12 1.13c Storm.dll rows (PKWARE implode body, CRT code page
// setup, locale info, small CRT helpers) transcribed from the cached decompiles. Stage A batch lane: no spec, no vector.
//
// The PKWARE work buffer is addressed by retail byte offset (st + 0x..); pointers inside it are offsets
// from st, so the retail pointer arithmetic is kept as written. The three 32-bit callback/param slots
// (0x9b0/0x9b4/0x9b8) hold a wasm table index / host token, widened through uintptr_t (host seam).
// Win32 and CRT services (GetCPInfo, _unlock, ___crtGetLocaleInfo*) are host seams passed in Host.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "../../../third_party/StormLib/src/pklib/pklib.h"   // DistBits/DistCode/ExLenBits/LenBits/LenCode/ChBitsAsc/ChCodeAsc

namespace {

typedef uint32_t (*PkIo)(uint8_t* buf, uint32_t* size, void* param);

inline uint32_t& U32(uint8_t* st, uint32_t o) { return *(uint32_t*)(st + o); }
inline uint16_t& U16(uint8_t* st, uint32_t o) { return *(uint16_t*)(st + o); }
inline int16_t&  S16(uint8_t* st, uint32_t o) { return *(int16_t*)(st + o); }
inline PkIo Io(uint8_t* st, uint32_t o) { return (PkIo)(uintptr_t)U32(st, o); }

// 1.13c Storm+0xFC40
void sub_FC40(uint8_t* st) {
    uint32_t n = 0x800;
    Io(st, 0x9b8)(st + 0x1fca, &n, (void*)(uintptr_t)U32(st, 0x9b0));
    uint8_t a = st[0x27ca], b = st[U32(st, 4) + 0x1fca];
    U32(st, 4) -= 0x800;
    memset(st + 0x1fca, 0, 0x802);
    if (U32(st, 4) != 0) st[0x1fca] = a;
    if (U32(st, 8) != 0) st[U32(st, 4) + 0x1fca] = b;
}

// 1.13c Storm+0xFB40
void sub_FB40(uint8_t* st, uint32_t nbits, uint32_t code) {
    if (8 < nbits) { nbits -= 8; sub_FB40(st, 8, code); code >>= 8; }
    uint8_t sh = (uint8_t)U32(st, 8);
    uint32_t cnt = U32(st, 8) + nbits;
    st[U32(st, 4) + 0x1fca] |= (uint8_t)((char)code << (sh & 0x1f));
    U32(st, 8) = cnt;
    if (cnt < 9) {
        U32(st, 8) = cnt & 7;
        if ((cnt & 7) == 0) U32(st, 4)++;
    } else {
        uint32_t p = U32(st, 4)++;
        st[p + 0x1fcb] = (uint8_t)(code >> ((8 - sh) & 0x1f));
        U32(st, 8) &= 7;
    }
    if (0x7ff < U32(st, 4)) sub_FC40(st);
}

// 1.13c Storm+0xFCC0
void sub_FCC0(uint8_t* st, uint32_t p2, uint32_t p3) {
    memset(st + 0xdc8, 0, 0x1200);
    uint32_t p = p2, p1;
    do { p1 = p + 1; U16(st, st[p1] * 10 + st[p] * 8 + 0xdc8)++; p = p1; } while (p1 < p3);
    uint16_t sum = 0;
    for (uint32_t o = 0xdc8; o < 0x1fc8; o += 2) { sum += U16(st, o); U16(st, o) = sum; }
    p = p3 - 1;
    do {
        p1 = p + 1;
        uint8_t b = st[p];
        uint32_t old = p--;
        uint16_t i = --U16(st, b * 8 + st[p1] * 10 + 0xdc8);
        U16(st, 0x49d0 + i * 2) = (uint16_t)(old - 0x27cc);
    } while (p2 <= p);
}

// 1.13c Storm+0xF800
int sub_F800(uint8_t* st, uint32_t o) {
    int iVar1 = 1, iVar5;
    uint32_t d6 = U32(st, 0x18);
    uint32_t p6 = (st[o + 1] * 5 + st[o] * 4) * 2 + 0xdc8;
    uint32_t lc = U16(st, p6);
    uint32_t p3 = lc * 2 + 0x49d0;
    int64_t lim = (int64_t)o - d6 - 0x27cb;
    if ((int64_t)U16(st, p3) < lim) {
        do { p3 += 2; lc++; } while ((int64_t)U16(st, p3) < lim);
        U16(st, p6) = (uint16_t)lc;
    }
    uint32_t p4 = o - 1;
    p6 = lc * 2 + 0x49d0;
    uint32_t p7 = U16(st, p6) + 0x27cc;
    if (p4 <= p7) return 0;
    for (;;) {
        if (st[p7 + iVar1 - 1] == st[o + iVar1 - 1] && st[p7] == st[o]) {
            p7++; iVar5 = 2; uint32_t p2 = o;
            do { p7++; if (st[p7] != st[p2 + 2]) break; iVar5++; p2++; } while (iVar5 < 0x204);
            if (iVar1 <= iVar5) {
                U32(st, 0) = o + iVar5 - p7 - 1; iVar1 = iVar5;
                if (10 < iVar5) {
                    if (iVar5 == 0x204) { U32(st, 0)--; return 0x204; }
                    if (p4 <= (uint32_t)U16(st, lc * 2 + 0x49d2) + 0x27cc) return iVar5;
                    int16_t s8 = 0, l12 = 1;
                    S16(st, 0x9bc) = -1; S16(st, 0x9be) = 0;
                    do {
                        if (st[o + l12] == st[o + s8] || (s8 = S16(st, 0x9bc + s8 * 2), s8 == -1)) {
                            l12++; s8++; S16(st, 0x9bc + l12 * 2) = s8;
                        }
                    } while (l12 < iVar5);
                    uint32_t l10 = U16(st, lc * 2 + 0x49d0) + iVar5 + 0x27cc;
                    for (;;) {
                        iVar5 = S16(st, 0x9bc + iVar5 * 2);
                        if (iVar5 == -1) iVar5 = 0;
                        p6 = lc * 2 + 0x49d0;
                        do {
                            p6 += 2; lc++; p7 = U16(st, p6) + 0x27cc;
                            if (p4 <= p7) return iVar1;
                        } while (p7 + iVar5 < l10);
                        if (st[p7 + iVar1 - 2] == st[o + iVar1 - 2]) {
                            if (p7 + iVar5 != l10) { iVar5 = 0; l10 = p7; }
                        } else {
                            p6 = lc * 2 + 0x49d0;
                            do {
                                p6 += 2; lc++; p7 = U16(st, p6) + 0x27cc;
                                if (p4 <= p7) return iVar1;
                            } while (st[p7 + iVar1 - 2] != st[o + iVar1 - 2] || st[o] != st[p7]);
                            iVar5 = 2; l10 = p7 + 2;
                        }
                        if (st[o + iVar5] == st[l10]) {
                            do { iVar5++; if (0x203 < iVar5) break; l10++; } while (st[o + iVar5] == st[l10]);
                        }
                        if (iVar1 <= iVar5) {
                            U32(st, 0) = o - 1 - p7;
                            if (iVar1 < iVar5) {
                                if (iVar5 == 0x204) return 0x204;
                                do {
                                    if (st[o + l12] == st[o + s8] || (s8 = S16(st, 0x9bc + s8 * 2), s8 == -1)) {
                                        l12++; s8++; S16(st, 0x9bc + l12 * 2) = s8;
                                    }
                                    iVar1 = iVar5;
                                } while (l12 < iVar5);
                            }
                        }
                    }
                }
            }
        }
        p6 += 2; lc++; p7 = U16(st, p6) + 0x27cc;
        if (p4 <= p7) { if (iVar1 < 2) iVar1 = 0; return iVar1; }
    }
}

// 1.13c Storm+0xF480
void sub_F480(uint8_t* st) {
    int local_4 = 0;
    bool eof = false;
    st[0x1fca] = (uint8_t)U32(st, 0x14);
    uint32_t p9 = U32(st, 0x18) + 0x29d0;
    st[0x1fcb] = (uint8_t)U32(st, 0xc);
    U32(st, 4) = 2;
    memset(st + 0x1fcc, 0, 0x800);
    U32(st, 8) = 0;
    for (;;) {
        int want = 0x1000, got = 0;
        uint32_t p10;
        do {
            uint32_t n = want;
            int r = (int)Io(st, 0x9b4)(st + U32(st, 0x18) + got + 0x29d0, &n, (void*)(uintptr_t)U32(st, 0x9b0));
            if (r == 0) {
                if (got == 0 && local_4 == 0) {
                    sub_FB40(st, st[0x3a1], (uint16_t)U32(st, 0x9ac));
                    if (U32(st, 8) != 0) U32(st, 4)++;
                    Io(st, 0x9b8)(st + 0x1fca, &U32(st, 4), (void*)(uintptr_t)U32(st, 0x9b0));
                    return;
                }
                eof = true;
                break;
            }
            got += r; want -= r;
        } while (want != 0);
        uint32_t d = U32(st, 0x18);
        p10 = d + got + 0x27cc;
        if (eof) p10 += 0x204;
        if (local_4 == 0) {
            sub_FCC0(st, p9, p10 + 1);
            local_4++;
            if (U32(st, 0x18) != 0x1000) local_4++;
        } else if (local_4 == 1) {
            sub_FCC0(st, p9 + (0x204 - d), p10 + 1);
            local_4++;
        } else {
            sub_FCC0(st, p9 - d, p10 + 1);
        }
        for (;;) {
            if (!(p9 < p10)) break;
            uint32_t len = sub_F800(st, p9);
            for (;;) {
                if (len == 0 || (len == 2 && 0xff < U32(st, 0))) goto literal;
                if (eof && p10 < p9 + len) {
                    len = p10 - p9;
                    if (1 < len && (len != 2 || U32(st, 0) < 0x100)) goto emit;
                    goto literal;
                }
                if (7 < len || p10 <= p9 + 1) goto emit;
                uint32_t save = U32(st, 0);
                uint32_t l2 = (uint32_t)sub_F800(st, p9 + 1);
                if (l2 <= len || (l2 <= len + 1 && save < 0x81)) { U32(st, 0) = save; goto emit; }
                uint8_t c = st[p9++];
                sub_FB40(st, st[c + 0x9c], U16(st, c * 2 + 0x3a2));
                len = l2;
            }
        emit:
            sub_FB40(st, st[len + 0x19a], U16(st, len * 2 + 0x59e));
            if (len == 2) {
                uint32_t dd = U32(st, 0) >> 2;
                sub_FB40(st, st[dd + 0x1c], st[dd + 0x5c]);
                p9 += 2;
                sub_FB40(st, 2, U32(st, 0) & 3);
            } else {
                uint32_t dd = U32(st, 0) >> (U32(st, 0xc) & 0x1f);
                sub_FB40(st, st[dd + 0x1c], st[dd + 0x5c]);
                p9 += len;
                sub_FB40(st, U32(st, 0xc), U32(st, 0x10) & U32(st, 0));
            }
            continue;
        literal: {
                uint8_t c = st[p9++];
                sub_FB40(st, st[c + 0x9c], U16(st, c * 2 + 0x3a2));
            }
        }
        if (eof) {
            sub_FB40(st, st[0x3a1], (uint16_t)U32(st, 0x9ac));
            if (U32(st, 8) != 0) U32(st, 4)++;
            Io(st, 0x9b8)(st + 0x1fca, &U32(st, 4), (void*)(uintptr_t)U32(st, 0x9b0));
            return;
        }
        p9 -= 0x1000;
        memmove(st + 0x27cc, st + 0x37cc, U32(st, 0x18) + 0x204);
    }
}

} // namespace

// 1.13c Storm+0xF2E0
extern "C" uint32_t storm_n13_F2E0(uint32_t readCb, uint32_t writeCb, uint8_t* st, uint32_t param, uint32_t* type, uint32_t* dsize) {
    U32(st, 0x9b4) = readCb; U32(st, 0x9b8) = writeCb;
    U32(st, 0x18) = *dsize; U32(st, 0x14) = *type; U32(st, 0x9b0) = param;
    U32(st, 0xc) = 4; U32(st, 0x10) = 0xf;
    if (*dsize != 0x400) {
        if (*dsize != 0x800) {
            if (*dsize != 0x1000) return 1;
            U32(st, 0xc)++; U32(st, 0x10) |= 0x20;
        }
        U32(st, 0xc)++; U32(st, 0x10) |= 0x10;
    }
    uint32_t n = 0;
    if (*type == 0) {
        for (; n < 0x100; n++) { st[0x9c + n] = 9; U16(st, 0x3a2 + n * 2) = (uint16_t)(n * 2); }
    } else {
        if (*type != 1) return 2;
        for (; n < 0x100; n++) { st[0x9c + n] = ChBitsAsc[n] + 1; U16(st, 0x3a2 + n * 2) = (uint16_t)(ChCodeAsc[n] * 2); }
    }
    for (uint32_t i = 0; i < 0x10; i++) {
        for (uint32_t k = 0; k < (1u << (ExLenBits[i] & 0x1f)); k++, n++) {
            st[0x9c + n] = ExLenBits[i] + LenBits[i] + 1;
            U16(st, 0x3a2 + n * 2) = (uint16_t)(k << ((LenBits[i] + 1) & 0x1f) | LenCode[i] * 2 | 1);
        }
    }
    memcpy(st + 0x5c, DistCode, 0x40);
    memcpy(st + 0x1c, DistBits, 0x40);
    sub_F480(st);
    return 0;
}

// ---- CRT helpers (host seams) ------------------------------------------------------------------
struct CpInfo { uint32_t maxCharSize; uint8_t leadByte[12]; };
struct Host {
    bool (*getCPInfo)(uint32_t cp, CpInfo* out);
    void (*unlock)(int n);                                      // Storm+0x902A (_unlock)
    int  (*getLocaleInfoA)(uint32_t lcid, uint32_t type, char* out, int n);   // ___crtGetLocaleInfoA, 0 on failure
    int  (*getLocaleInfoW)(uint32_t lcid, uint32_t type, uint16_t* out, int n); // ___crtGetLocaleInfoW
    uint32_t (*lastError)();                                    // GetLastError
};

struct CrtState {
    uint8_t  ctype[0x101];   // 0x6FC47960
    uint32_t cp, cpFlag, lcid, v70, v74, v78;   // 0x6FC47A64 0x6FC4794C 0x6FC47944 0x6FC47A70/74/78
    uint32_t haveMbcp;       // 0x6FC427B8 (zero: unknown code page is an error)
};

// Storm.dll 0x6FC407E8..: five CJK code page entries { cp, 3 words, 4 x 4 byte-pair ranges (flag 1,2,4,8) }
struct CpEntry { uint32_t cp, w[3]; uint8_t r[4][8]; };
const CpEntry kCp[5] = {
    {932,  {2189001312u, 33, 0}, {{0xa6,0xdf},{0xa1,0xa5},{0x81,0x9f,0xe0,0xfc},{0x40,0x7e,0x80,0xfc}}},
    {936,  {2749014977u, 32, 0},        {{0},{0},{0x81,0xfe},{0x40,0xfe}}},
    {949,  {2749014977u, 32, 0},        {{0},{0},{0x81,0xfe},{0x41,0xfe}}},
    {950,  {2732892879u, 2732916762u, 6005480u}, {{0},{0},{0x81,0xfe},{0x40,0x7e,0xa1,0xfe}}},
    {1361, {3663649361u, 3663659040u, 3332714u}, {{0},{0},{0x81,0xd3,0xd8,0xde,0xe0,0xf9},{0x31,0x7e,0x81,0xfe}}},
};
const uint8_t kFlag[4] = {1, 2, 4, 8};   // 0x6FC407E0

// 1.13c Storm+0xC052
uint32_t storm_n13_C052(int cp) {
    return cp == 0x3a4 ? 0x411 : cp == 0x3a8 ? 0x804 : cp == 0x3b5 ? 0x412 : cp == 0x3b6 ? 0x404 : 0;
}

// 1.13c Storm+0xC245 (_setmbcp helper); setSBCS = Storm+0xC081, tables = Storm+0xC0AA, both supplied by the caller
extern "C" int storm_n13_C245(CrtState& g, const Host& h, uint32_t cp, void (*setSBCS)(CrtState&), void (*tables)(CrtState&)) {
    if (cp != 0) {
        for (int i = 0; i < 5; i++) {
            if (kCp[i].cp != cp) continue;
            memset(g.ctype, 0, sizeof g.ctype);
            for (int grp = 0; grp < 4; grp++)
                for (const uint8_t* r = kCp[i].r[grp]; r[0] && r[1]; r += 2)
                    for (uint32_t c = r[0]; c <= r[1]; c++) g.ctype[c + 1] |= kFlag[grp];
            g.cp = cp; g.cpFlag = 1; g.lcid = storm_n13_C052((int)cp);
            g.v70 = kCp[i].w[0]; g.v74 = kCp[i].w[1]; g.v78 = kCp[i].w[2];
            tables(g);
            return 0;
        }
        CpInfo ci;
        if (h.getCPInfo(cp, &ci)) {
            memset(g.ctype, 0, sizeof g.ctype);
            g.cp = cp; g.lcid = 0;
            if (ci.maxCharSize < 2) g.cpFlag = 0;
            else {
                for (const uint8_t* b = ci.leadByte; b[0] && b[1]; b += 2)
                    for (uint32_t c = b[0]; c <= b[1]; c++) g.ctype[c + 1] |= 4;
                for (uint32_t c = 1; c < 0xff; c++) g.ctype[c + 1] |= 8;
                g.lcid = storm_n13_C052((int)cp);
                g.cpFlag = 1;
            }
            g.v70 = g.v74 = g.v78 = 0;
            tables(g);
            return 0;
        }
        if (g.haveMbcp == 0) return -1;
    }
    setSBCS(g);
    tables(g);
    return 0;
}

// 1.13c Storm+0xC520
extern "C" void storm_n13_C520(const Host& h) { h.unlock(0xd); }

// 1.13c Storm+0xE645 (in_ECX = lock number, saved = the caller's [EBP-0x1C])
extern "C" void storm_n13_E645(const Host& h, int saved, int lock) { if (saved != lock) h.unlock(lock); }

// 1.13c Storm+0xE4DC (EDX = table of 12-byte entries, ESI = signal number, count = 0x6FC4048C); returns the entry or the end
extern "C" const uint8_t* storm_n13_E4DC(const uint8_t* tab, int sig, uint32_t count) {
    const uint8_t* p = tab;
    do {
        if (*(const int32_t*)(p + 4) == sig) return p;
        p += 12;
    } while (p < tab + count * 12);
    return p;
}

// 1.13c Storm+0xDEAA (param_1: 0 = numeric byte from a wide string, 1 = malloc'd copy of the ANSI string)
extern "C" int storm_n13_DEAA(const Host& h, int mode, uint32_t lcid, uint32_t type, char* out) {
    if (mode != 1) {
        if (mode != 0) return -1;
        uint16_t w[4];
        if (h.getLocaleInfoW(lcid, type, w, 4) == 0) return -1;
        *out = 0;
        for (int i = 0; i < 4; i++) {
            uint8_t b = (uint8_t)w[i];
            if (b < '0' || b > '9') return 0;
            *out = (char)(*out * 10 + b - '0');
        }
        return 0;
    }
    char buf[0x80]; char* src = buf; bool big = false;
    int n = h.getLocaleInfoA(lcid, type, buf, 0x80);
    if (n == 0) {
        if (h.lastError() != 0x7a) return -1;
        int need = h.getLocaleInfoA(lcid, type, nullptr, 0);
        if (need == 0) return -1;
        src = (char*)malloc(need);
        if (!src) return -1;
        big = true;
        n = h.getLocaleInfoA(lcid, type, src, need);
        if (n == 0) { free(src); return -1; }
    }
    char* dst = (char*)malloc(n);
    *(char**)out = dst;
    if (dst) { strncpy(dst, src, n); if (big) free(src); return 0; }
    if (big) free(src);
    return -1;
}

// 1.13c Storm+0xFD80 (list head 0x6FC45A50; nodes link through a 32-bit slot at +4, wide key at +0x114)
uintptr_t g_n13_listHead;
extern "C" uintptr_t storm_n13_FD80(const uint16_t* key) {
    intptr_t n = (intptr_t)g_n13_listHead;
    while (0 < n) {
        const uint16_t* a = (const uint16_t*)(n + 0x114); const uint16_t* b = key;
        while (*a == *b && *a) { a++; b++; }
        if (*a == *b) return (uintptr_t)n;
        n = (intptr_t)*(uint32_t*)(n + 4);
    }
    return 0;
}
