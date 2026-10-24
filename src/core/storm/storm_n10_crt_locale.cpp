// src/core/storm/storm_n10_crt_locale.cpp -- 12 1.13c Storm.dll statically linked MSVC CRT rows
// (command-line parse, locale/codepage setup). Transcribed from the cached decompiles. Retail
// register-only arguments (in_EAX/in_ECX/unaff_ESI) are explicit parameters. CRT globals are one
// struct (retail addresses in comments); Win32 services (GetCPInfo, GetStringType, LCMapString,
// GetLocaleInfoA, the locale getter pointer at 0x6FC4276C) are host seams passed in Host.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <strings.h>

namespace {

struct CpInfo { uint32_t maxCharSize; uint8_t leadByte[12]; };

struct Host {
    bool (*getCPInfo)(uint32_t cp, CpInfo* out);
    bool (*getStringTypeA)(uint32_t lcid, const char* s, int n, uint16_t* out, uint32_t cp);
    int  (*lcMapString)(uint32_t lcid, uint32_t flags, const char* s, int n, char* out, int outN, uint32_t cp);
    int  (*getLocaleInfoA)(uint32_t lcid, uint32_t type, char* out, int n);
    void (*crtExit)(int code);
};

struct CrtState {
    uint8_t  ctype[0x101];   // 0x6FC47960 (ctype[c+1] flags, bit2 = DBCS lead byte)
    uint8_t  lcmap[0x100];   // 0x6FC47A80
    uint32_t cp;             // 0x6FC47A64
    uint32_t cpOem, lcid, v47a70, v47a74, v47a78; // 0x6FC4794C 0x6FC47944 0x6FC47A70/74/78
    uint32_t mbcsOn;         // 0x6FC42778
    uint32_t mbCodePage;     // 0x6FC42788
    uint32_t mbCodePageReq;  // 0x6FC427A0
    uint32_t mbCurMax;       // 0x6FC406B0
    const uint8_t* ptrCtype; // 0x6FC406A8
    const uint16_t* ptr47b80; // 0x6FC47B80
    const uint32_t* ptr47b84; // 0x6FC47B84
    // locale search state
    uint32_t flags;          // 0x6FC4274C
    uint32_t lcidA, lcidB;   // 0x6FC42750 0x6FC42754
    uint32_t flagLang;       // 0x6FC4275C
    uint32_t flagCtry;       // 0x6FC42758
    const char* langName;    // 0x6FC42760
    const char* ctryName;    // 0x6FC42768
    uint32_t ctryLen;        // 0x6FC42764
    int (*getLocale)(uint32_t lcid, uint32_t type, char* out, int n); // 0x6FC4276C
};

// 0x6FC3487C: ten LCIDs excluded from the fallback choice
const int16_t kExcluded[10] = {0x0409, 0x0809, 0x0C09, 0x1009, 0x1409, 0x1809, 0x1C09, 0x2009, 0x2409, 0x2809};

// _LcidFromHexString (0x...B542): hex digits to value
uint32_t LcidFromHex(const char* s) {
    uint32_t v = 0;
    for (; *s; s++) {
        int c = (unsigned char)*s, d;
        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
        else break;
        v = v * 16 + d;
    }
    return v;
}

// _GetPrimaryLen: leading alphabetic run
size_t PrimaryLen(const char* s) { size_t n = 0; while ((s[n] | 0x20) >= 'a' && (s[n] | 0x20) <= 'z') n++; return n; }

} // namespace

extern "C" {

// 1.13c Storm+0x8F83
void storm_8F83(const Host& h) { h.crtExit(4); }

// 1.13c Storm+0x9580  (_parse_cmdline; in_EAX=cmd, in_ECX=args buffer, unaff_ESI=&charCount)
void storm_9580(const CrtState& g, const uint8_t* p, uint8_t* args, uint8_t** argv, int* argc, int* chars) {
    bool inq = false;
    *chars = 0; *argc = 1;
    if (argv) *argv++ = args;
    uint8_t c;
    for (;;) {
        const uint8_t* n;
        if (*p == 0x22) { inq = !inq; n = p + 1; c = 0x22; }
        else {
            ++*chars;
            if (args) *args++ = *p;
            c = *p; n = p + 1;
            if (g.ctype[c + 1] & 4) { ++*chars; if (args) *args++ = *n; n = p + 2; }
            if (c == 0) { p = n - 1; goto rest; }
        }
        p = n;
        if (!inq && (c == 0x20 || c == 9)) break;
    }
    if (args) args[-1] = 0;
rest:
    inq = false;
    while (*p) {
        while (*p == 0x20 || *p == 9) p++;
        if (!*p) break;
        if (argv) *argv++ = args;
        ++*argc;
        for (;;) {
            bool copy = true; uint32_t bs = 0;
            for (; *p == 0x5c; p++) bs++;
            if (*p == 0x22) {
                const uint8_t* q = p;
                if (!(bs & 1)) {
                    if (!inq || *(q = p + 1) != 0x22) { copy = false; q = p; }
                    inq = !inq;
                }
                bs >>= 1; p = q;
            }
            for (; bs; bs--) { if (args) *args++ = 0x5c; ++*chars; }
            c = *p;
            if (c == 0 || (!inq && (c == 0x20 || c == 9))) break;
            if (copy) {
                if (!args) { if (g.ctype[c + 1] & 4) { p++; ++*chars; } }
                else {
                    if (g.ctype[c + 1] & 4) { *args++ = c; p++; ++*chars; }
                    *args++ = *p;
                }
                ++*chars;
            }
            p++;
        }
        if (args) *args++ = 0;
        ++*chars;
    }
    if (argv) *argv = nullptr;
    ++*argc;
}

// 1.13c Storm+0xAC37  (_setmbcp ctype tables; 0 ok, 1 fail)
uint32_t storm_AC37(CrtState& g, const Host& h) {
    if (!g.mbcsOn) { g.ptrCtype = nullptr; g.ptr47b80 = nullptr; g.ptr47b84 = nullptr; return 0; }
    uint32_t* cnt = (uint32_t*)malloc(4);
    uint8_t* tbl = (uint8_t*)malloc(0x300);
    uint8_t* ident = (uint8_t*)malloc(0x101);
    uint32_t rc = 1;
    if (cnt && tbl && ident) {
        *cnt = 0;
        for (int i = 0; i < 0x100; i++) ident[i] = (uint8_t)i;
        CpInfo ci;
        if (h.getCPInfo(g.mbCodePage, &ci) && ci.maxCharSize < 6) {
            g.mbCurMax = ci.maxCharSize;
            if (g.mbCurMax > 1)
                for (const uint8_t* b = ci.leadByte; b[0] && b[1]; b += 2)
                    for (uint32_t u = b[0]; u <= b[1]; u++) ident[u] = 0;
            if (h.getStringTypeA(1, (const char*)ident, 0x100, (uint16_t*)(tbl + 0x100), 0)) {
                *(uint16_t*)(tbl + 0xFE) = 0;
                if (g.mbCurMax > 1)
                    for (const uint8_t* b = ci.leadByte; b[0] && b[1]; b += 2)
                        for (uint32_t u = b[0]; u <= b[1]; u++) *(uint16_t*)(tbl + 0x100 + u * 2) = 0x8000;
                memcpy(tbl, tbl + 0x200, 0xFE);
                g.ptrCtype = tbl + 0x100; g.ptr47b80 = (uint16_t*)(tbl + 0xFE); g.ptr47b84 = cnt;
                rc = 0;
            }
        }
    }
    if (rc) { free(cnt); free(tbl); }
    free(ident);
    return rc;
}

// 1.13c Storm+0xB441  (1 when lcid is not in the excluded list)
uint32_t storm_B441(int16_t lcid) {
    for (int i = 0; i < 10; i++) if (lcid == kExcluded[i]) return 0;
    return 1;
}

// 1.13c Storm+0xB45F  (_crtGetLocaleInfoA; table = 0x6FC34260, 27 entries of 0x2C bytes)
int storm_B45F(const Host& h, const uint8_t* table, uint32_t lcid, uint32_t type, char* out, int n) {
    int lo = 0, hi = 0x1a;
    do {
        int m = (hi + lo) / 2;
        const uint8_t* e = table + m * 0x2c;
        uint32_t key; memcpy(&key, e, 4);
        if (lcid == key) {
            const char* src;
            switch (type) {
            case 1: src = (const char*)e + 4; break;
            case 3: src = (const char*)e + 0x10; break;
            case 7: src = (const char*)e + 0x18; break;
            case 0xb: src = (const char*)e + 0x1c; break;
            case 0x1001: memcpy(&src, e + 0xc, sizeof src); break;
            case 0x1002: memcpy(&src, e + 0x14, sizeof src); break;
            case 0x1004: src = (const char*)e + 0x24; break;
            default: goto fallback;
            }
            if (src && n > 0) { strncpy(out, src, n - 1); out[n - 1] = 0; return 1; }
            break;
        }
        if (lcid < key) hi = m - 1; else lo = m + 1;
    } while (lo <= hi);
fallback:
    return h.getLocaleInfoA(lcid, type, out, n);
}

// type selectors: retail computes (-(flag!=0) & 0xFFFFF005) + 0x1002, i.e. 7 when set else 0x1002
static inline uint32_t LangType(const CrtState& g) { return g.flagLang ? 7 : 0x1002; }
static inline uint32_t CtryType(const CrtState& g) { return g.flagCtry ? 3 : 0x1001; }

// 1.13c Storm+0xB594  (language-only locale enum callback; this=ECX; 1 continue, 0 stop)
uint32_t storm_B594(CrtState& g, const char* hex) {
    char buf[120];
    uint32_t id = LcidFromHex(hex);
    if (!g.getLocale(id, LangType(g), buf, 0x78)) { g.flags = 0; return 1; }
    if (strcasecmp(g.langName, buf) == 0 && storm_B441((int16_t)id)) {
        g.flags |= 4; g.lcidA = id; g.lcidB = id;
    }
    return ~(g.flags >> 2) & 1;
}

// 1.13c Storm+0xB62C  (does the primary-language locale of lcid match; 1 yes)
uint32_t storm_B62C(CrtState& g, uint32_t lcid, int strict) {
    char buf[120];
    if (!g.getLocale(lcid & 0x3ff | 0x400, 1, buf, 0x78)) return 0;
    if (lcid != LcidFromHex(buf) && strict && PrimaryLen(g.ctryName) == strlen(g.ctryName)) return 0;
    return 1;
}

// 1.13c Storm+0xB6A1  (full language+country locale enum callback; this=ECX)
uint32_t storm_B6A1(CrtState& g, const char* hex) {
    char buf[120];
    uint32_t id = LcidFromHex(hex);
    if (!g.getLocale(id, LangType(g), buf, 0x78)) { g.flags = 0; return 1; }
    if (strcasecmp(g.langName, buf) == 0) {
        if (!g.getLocale(id, CtryType(g), buf, 0x78)) { g.flags = 0; return 1; }
        if (strcasecmp(g.ctryName, buf) == 0) {
            g.flags |= 0x304; g.lcidA = id; g.lcidB = id;
        } else if (!(g.flags & 2)) {
            if (g.ctryLen && strncasecmp(g.ctryName, buf, g.ctryLen) == 0) {
                g.flags |= 2; g.lcidB = id;
                if (strlen(g.ctryName) == g.ctryLen) g.lcidA = id;
            } else if (!(g.flags & 1) && storm_B441((int16_t)id)) {
                g.flags |= 1; g.lcidB = id;
            }
        }
    }
    if ((g.flags & 0x300) != 0x300) {
        if (!g.getLocale(id, CtryType(g), buf, 0x78)) { g.flags = 0; return 1; }
        int strict = -1;
        if (strcasecmp(g.ctryName, buf) == 0) {
            g.flags |= 0x200;
            if (!g.flagCtry && g.ctryLen && strlen(g.ctryName) == g.ctryLen) strict = 1;
        } else if (!g.flagCtry && g.ctryLen && strncasecmp(g.ctryName, buf, g.ctryLen) == 0) strict = 0;
        if (strict >= 0 ? storm_B62C(g, id, strict) != 0 : strcasecmp(g.ctryName, buf) == 0) {
            g.flags |= 0x100;
            if (!g.lcidA) g.lcidA = id;
        }
    }
    return ~(g.flags >> 2) & 1;
}

// 1.13c Storm+0xB8BF  (country-only locale enum callback; this=ECX)
uint32_t storm_B8BF(CrtState& g, const char* hex) {
    char buf[120];
    uint32_t id = LcidFromHex(hex);
    if (!g.getLocale(id, CtryType(g), buf, 0x78)) { g.flags = 0; return 1; }
    int strict;
    if (strcasecmp(g.ctryName, buf) == 0) { if (g.flagCtry) goto hit; strict = 1; }
    else if (g.flagCtry || !g.ctryLen || strncasecmp(g.ctryName, buf, g.ctryLen) != 0) goto done;
    else strict = 0;
    if (!storm_B62C(g, id, strict)) goto done;
hit:
    g.flags |= 4; g.lcidA = id; g.lcidB = id;
done:
    return ~(g.flags >> 2) & 1;
}

// 1.13c Storm+0xC052  (in_EAX = codepage; maps DBCS ANSI code pages to LANGID)
uint32_t storm_C052(int cp) {
    return cp == 0x3a4 ? 0x411 : cp == 0x3a8 ? 0x804 : cp == 0x3b5 ? 0x412 : cp == 0x3b6 ? 0x404 : 0;
}

// 1.13c Storm+0xC081  (_setSBCS)
void storm_C081(CrtState& g) {
    memset(g.ctype, 0, sizeof g.ctype);
    g.cp = 0; g.cpOem = 0; g.lcid = 0; g.v47a70 = 0; g.v47a74 = 0; g.v47a78 = 0;
}

// 1.13c Storm+0xC0AA  (case/ctype tables for the current code page)
void storm_C0AA(CrtState& g, const Host& h) {
    CpInfo ci;
    if (h.getCPInfo(g.cp, &ci)) {
        char chars[256]; uint16_t type[256]; char upper[256], lower[256];
        for (int i = 0; i < 256; i++) chars[i] = (char)i;
        chars[0] = ' ';
        for (const uint8_t* b = ci.leadByte; b[0] && b[1]; b += 2)
            for (uint32_t u = b[0]; u <= b[1]; u++) chars[u] = ' ';
        h.getStringTypeA(1, chars, 0x100, type, g.cp);
        h.lcMapString(g.lcid, 0x100, chars, 0x100, upper, 0x100, g.cp);
        h.lcMapString(g.lcid, 0x200, chars, 0x100, lower, 0x100, g.cp);
        for (int i = 0; i < 256; i++) {
            if (type[i] & 1) { g.ctype[i + 1] |= 0x10; g.lcmap[i] = (uint8_t)upper[i]; }
            else if (type[i] & 2) { g.ctype[i + 1] |= 0x20; g.lcmap[i] = (uint8_t)lower[i]; }
            else g.lcmap[i] = 0;
        }
    } else {
        for (uint32_t i = 0; i < 256; i++) {
            if (i >= 0x41 && i <= 0x5a) { g.ctype[i + 1] |= 0x10; g.lcmap[i] = (uint8_t)(i + 0x20); }
            else if (i > 0x60 && i < 0x7b) { g.ctype[i + 1] |= 0x20; g.lcmap[i] = (uint8_t)(i - 0x20); }
            else g.lcmap[i] = 0;
        }
    }
}

} // extern "C"
