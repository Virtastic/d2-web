// src/core/fog/fog_batch_z12.cpp -- twelve 1.13c Fog.dll statically linked MSVC CRT rows, from the
// cached Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c). Alpha lane: no spec/vector.
// Retail globals and callees are seam parameters; register-passed implicit args are explicit.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace {

// 1.13c Fog+0x0AFAD _atol. seam: isspace(c) is the locale ctype test (_ISSPACE = 8).
int32_t atol_z12(const char* s, int (*isspace_seam)(unsigned c)) {
    while (isspace_seam((uint8_t)*s)) s++;
    unsigned c = (uint8_t)*s, sign = c;
    const uint8_t* p = (const uint8_t*)s + 1;
    if (c == 0x2d || c == 0x2b) { c = *p; p++; }
    int32_t v = 0;
    while (c >= 0x30 && c <= 0x39) { v = (int32_t)(c - 0x30) + v * 10; c = *p++; }
    return sign == 0x2d ? -v : v;
}

// 1.13c Fog+0x0B334 siglookup. tab = 12-byte entries (+4 = signal number), n = DAT_6ff7e7e4.
// Returns the matching entry index; like retail, runs off the end (index n) when absent.
uint32_t siglookup_z12(const uint32_t* tab, uint32_t n, int32_t sig) {
    uint32_t i = 0;
    do {
        if ((int32_t)tab[i * 3 + 1] == sig) return i;
        i++;
    } while (i < n);
    return i;
}

// Seams for _raise: per-thread state fields (local_28) and the retail globals.
struct RaiseSeam_z12 {
    uint32_t* hSigint;     // DAT_6ff806b0
    uint32_t* hSigterm;    // DAT_6ff806bc
    uint32_t* hSigbreak;   // DAT_6ff806b4
    uint32_t* hSigabrt;    // DAT_6ff806b8
    uint32_t* tab;         // exception action table base (ptd->_pxcptacttab-owned entries, 3 words)
    uint32_t tabCount;     // DAT_6ff7e7e4
    int32_t fpeFirst;      // DAT_6ff7e7d8
    int32_t fpeCount;      // DAT_6ff7e7dc
    uint32_t* ptdInitarg;  // ptd->_initarg
    uint32_t* ptdXcptact;  // ptd->_pxcptacttab
    uint32_t* ptdFpeTab;   // ptd->_initaddr entries, 3 words each
    void (*exit_)(int);    // __exit
    void (*lock)(int);     // __lock
    void (*unlock)(int);   // FUN_6ff5b49d
    void (*call1)(uint32_t h, int sig);
    void (*call2)(uint32_t h, int sig, uint32_t arg);
};

// 1.13c Fog+0x0B362 _raise
int raise_z12(int sig, RaiseSeam_z12& s) {
    int32_t simple = 0;
    uint32_t* slot = nullptr;
    uint32_t h = 0;
    if (sig == 2) { slot = s.hSigint; simple = 1; }
    else if (sig != 4 && sig != 8 && sig != 0xb) {
        if (sig == 0xf) slot = s.hSigterm;
        else if (sig == 0x15) slot = s.hSigbreak;
        else if (sig == 0x16) slot = s.hSigabrt;
        else return -1;
        simple = 1;
    } else {
        uint32_t i = siglookup_z12(s.tab, s.tabCount, sig);
        slot = &s.tab[i * 3 + 2];
    }
    h = *slot;
    if (h == 1) return 0;
    if (h == 0) s.exit_(3);
    if (simple) s.lock(0);
    uint32_t saveInit = 0, saveX = 0;
    if (sig == 8 || sig == 0xb || sig == 4) {
        saveInit = *s.ptdInitarg; *s.ptdInitarg = 0;
        if (sig == 8) { saveX = *s.ptdXcptact; *s.ptdXcptact = 0x8c; }
    }
    if (sig == 8) {
        for (int32_t i = s.fpeFirst; i < s.fpeCount + s.fpeFirst; i++) s.ptdFpeTab[i * 3 + 2] = 0;
    } else {
        *slot = 0;
    }
    s.unlock(0);
    if (sig == 8) s.call2(h, 8, *s.ptdXcptact);
    else {
        s.call1(h, sig);
        if (sig != 0xb && sig != 4) return 0;
    }
    *s.ptdInitarg = saveInit;
    if (sig == 8) *s.ptdXcptact = saveX;
    return 0;
}

// 1.13c Fog+0x0B57C __fassign. seam: strtod-style parsers (FUN_6ff5bd61 double, FUN_6ff5bda6 float).
void fassign_z12(int flag, uint32_t* arg, const char* num,
                 void (*cvtDouble)(uint32_t out[2], const char*), void (*cvtFloat)(uint32_t* out, const char*)) {
    if (flag) { uint32_t d[2]; cvtDouble(d, num); arg[0] = d[0]; arg[1] = d[1]; }
    else { uint32_t f; cvtFloat(&f, num); arg[0] = f; }
}

// 1.13c Fog+0x0B5BA __shift (EAX=str, EDI=n): move string (with NUL) up by n bytes.
void shift_z12(char* str, int n) {
    if (n != 0) memmove(str + n, str, strlen(str) + 1);
}

// _strcpy-ish seam used by __cftoe2 (FUN_6ff58e70 = strcpy returning dest end? returns dest).
// 1.13c Fog+0x0B5D7 __cftoe2. in = _strflt {sign@0, decpt@4, ?@8, mantissa ptr@12} (EAX), buf (EBX).
struct StrFlt_z12 { int32_t sign, decpt, flag; const char* mant; };
void cftoe2_z12(char* buf, StrFlt_z12* in, int ndigit, int caps, char shiftFlag, char decimalPoint) {
    if (shiftFlag) shift_z12(buf, 1);
    if (in->sign == 0x2d) *buf++ = '-';
    char* p = buf;
    if (ndigit > 0) { p = buf + 1; *buf = *p; *p = decimalPoint; }
    char* e = p + ndigit + (shiftFlag == 0);
    memcpy(e, "e+000", 6);
    if (caps) *e = 'E';
    if (in->mant[0] != '0') {
        int v = in->decpt - 1;
        if (v < 0) { v = -v; e[1] = '-'; }
        if (v > 99) { e[2] += (char)(v / 100); v %= 100; }
        if (v > 9) { e[3] += (char)(v / 10); v %= 10; }
        e[4] += (char)v;
    }
}

// 1.13c Fog+0x0B6F5 __cftof2. in as above (EAX); returns buf.
char* cftof2_z12(char* buf, StrFlt_z12* in, int ndigit, char shiftFlag, char decimalPoint) {
    int d = in->decpt;
    if (shiftFlag && (uint32_t)(d - 1) == (uint32_t)ndigit) {
        buf[(in->sign == 0x2d) + (uint32_t)(d - 1)] = '0';
        buf[(in->sign == 0x2d) + (uint32_t)(d - 1) + 1] = 0;
    }
    char* p = buf;
    if (in->sign == 0x2d) *p++ = '-';
    if (in->decpt < 1) { shift_z12(p, 1); *p++ = '0'; }
    else p += in->decpt;
    if (ndigit > 0) {
        shift_z12(p, 1);
        *p = decimalPoint;
        if (in->decpt < 0) {
            int n = -in->decpt;
            if (shiftFlag || n <= ndigit) ndigit = n;
            shift_z12(p + 1, ndigit);
            memset(p + 1, '0', (size_t)ndigit);
        }
    }
    return buf;
}

// 1.13c Fog+0x0BB2E __CopyMan: copy a 3-dword mantissa.
void CopyMan_z12(uint32_t* dst, const uint32_t* src) { for (int i = 0; i < 3; i++) dst[i] = src[i]; }

// 1.13c Fog+0x0BB49 __IsZeroMan
uint32_t IsZeroMan_z12(const uint32_t* m) {
    for (int i = 0; i < 3; i++) if (m[i]) return 0;
    return 1;
}

// Params table for __ld12cvt (DAT_6ff7ed20 double / DAT_6ff7ed38 float): 6 int32.
struct LdParams_z12 { int32_t maxExp, minExp, roundBits, manBits, totalBits, bias; };

// 1.13c Fog+0x0BBDD __ld12cvt. ld = 12-byte long double; seams RoundMan (0xBABC) and ShrMan (0xBB62).
uint32_t ld12cvt_z12(const uint16_t* ld, uint32_t* out, const LdParams_z12* P,
                     int (*RoundMan)(uint32_t* m, int bits), void (*ShrMan)(uint32_t* m, int n)) {
    uint16_t se = ld[5];
    uint32_t man[3];       // local_10, local_c, local_8 contiguous
    uint32_t save[3];
    memcpy(&man[0], ld + 3, 4);
    memcpy(&man[1], ld + 1, 4);
    man[2] = (uint32_t)ld[0] << 16;
    uint32_t e = se & 0x7fff;
    int32_t ex = (int32_t)e - 0x3fff;
    uint32_t status;
    if (ex == -0x3fff) {
        ex = 0;
        if (IsZeroMan_z12(man)) { status = 0; goto fin; }
        man[0] = 0; man[1] = 0;
    } else {
        CopyMan_z12(save, man);
        if (RoundMan(man, P->roundBits)) ex = (int32_t)e - 0x3ffe;
        int32_t mn = P->minExp;
        if (ex < mn - P->roundBits) { man[0] = 0; man[1] = 0; }
        else {
            if (mn < ex) {
                if (P->maxExp <= ex) {
                    man[1] = 0; man[2] = 0; man[0] = 0x80000000u;
                    ShrMan(man, P->manBits);
                    ex = P->bias + P->maxExp;
                    status = 1; goto fin;
                }
                man[0] &= 0x7fffffffu;
                ex = P->bias + ex;
                ShrMan(man, P->manBits);
                status = 0; goto fin;
            }
            CopyMan_z12(man, save);
            ShrMan(man, mn - ex);
            RoundMan(man, P->roundBits);
            ShrMan(man, P->manBits + 1);
        }
    }
    ex = 0; status = 2;
fin:
    man[0] = ((uint32_t)ex << ((0x1fu - (uint32_t)P->manBits) & 0x1f)) |
             ((se & 0x8000) ? 0x80000000u : 0u) | man[0];
    if (P->totalBits == 0x40) { out[1] = man[0]; out[0] = man[1]; }
    else if (P->totalBits == 0x20) out[0] = man[0];
    return status;
}

// 1.13c Fog+0x0BD35 FID_conflict:__ld12tod (params = DAT_6ff7ed20)
uint32_t ld12tod_z12(const uint16_t* ld, uint32_t* d, const LdParams_z12* DAT_ed20,
                     int (*R)(uint32_t*, int), void (*S)(uint32_t*, int)) {
    return ld12cvt_z12(ld, d, DAT_ed20, R, S);
}

// 1.13c Fog+0x0BD4B FID_conflict:__ld12tod / __ld12tof (params = DAT_6ff7ed38)
uint32_t ld12tof_z12(const uint16_t* ld, uint32_t* f, const LdParams_z12* DAT_ed38,
                     int (*R)(uint32_t*, int), void (*S)(uint32_t*, int)) {
    return ld12cvt_z12(ld, f, DAT_ed38, R, S);
}

}  // namespace
