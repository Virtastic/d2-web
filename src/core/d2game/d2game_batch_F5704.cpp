// MSVC CRT float-format and C++ EH helpers of D2Game 1.13c. Register inputs (EAX/EBX) are explicit
// parameters. SEH/C++ EH cannot exist in wasm, so the EH helpers keep their bookkeeping and drop the
// native frame calls (documented per function). No caller in the wasm build reaches them.
#include <cstdint>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>

struct CrtStrFlt { int sign; int decpt; int flag; char* mantissa; }; // STRFLT: sign, decpt, flag, mantissa ptr
static char g_decimalPoint = '.'; // DAT_6fd1f994 (locale decimal point)

static void crtShift(char* p) { memmove(p + 1, p, strlen(p) + 1); } // __shift: move string one char right

// 1.13c D2Game+0xF5704 __fpmath: x87 init + default precision; no x87 control word in wasm.
void D2GAME_F5704_fpmath(int setDefaultPrecision) { (void)setDefaultPrecision; }

// 1.13c D2Game+0xF5806 ___FrameUnwindToState: walks the unwind map; the native destructor call
// (__CallSettingFrame) has no wasm equivalent, so only the state walk is kept. frame[2] = state.
void D2GAME_F5806_FrameUnwindToState(int32_t* frame, int32_t funcInfoMaxState, const int32_t* unwindMap, int32_t target)
{
    int32_t state = frame[2];
    while (state != target) {
        if (state < 0 || funcInfoMaxState <= state) abort(); // _inconsistency
        int32_t next = unwindMap[state * 2];
        if (unwindMap[state * 2 + 1] != 0) frame[2] = next; // action would run here
        state = next;
    }
    frame[2] = state;
}

// 1.13c D2Game+0xF58D4 ___DestructExceptionObject: no C++ exceptions in wasm; nothing to destruct.
void D2GAME_F58D4_DestructExceptionObject(void* exceptionObject) { (void)exceptionObject; }

// 1.13c D2Game+0xF5FA1 ___InternalCxxFrameHandler: SEH dispatch; wasm has none. Returns
// ExceptionContinueSearch (1) like the non-matching path of retail.
int D2GAME_F5FA1_InternalCxxFrameHandler() { return 1; }

// 1.13c D2Game+0xF6050 __CallSettingFrame@12: NLG notify + indirect call of a funclet; native only.
void D2GAME_F6050_CallSettingFrame(void (*funclet)()) { if (funclet) funclet(); }

// 1.13c D2Game+0xF609C: insert the locale decimal point before the exponent 'e' position.
void D2GAME_F609C_ForceDecimalPoint(char* s)
{
    while (tolower((unsigned char)*s) != 'e') { ++s; if (!isdigit((unsigned char)*s)) break; }
    // retail loop: advance while next char is a digit, then shift tail right inserting '.'
    char c = *s; *s = g_decimalPoint;
    do { ++s; char n = *s; *s = c; c = n; } while (*s != '\0');
}

// 1.13c D2Game+0xF613D __fassign: string -> float/double store. strtod replaces the CRT parsers.
void D2GAME_F613D_fassign(int isDouble, char* arg, const char* number)
{
    if (isDouble) { double d = strtod(number, nullptr); memcpy(arg, &d, 8); }
    else { float f = (float)strtod(number, nullptr); memcpy(arg, &f, 4); }
}

// 1.13c D2Game+0xF6198 __cftoe2 (in_EAX = flt, EBX = buf)
void D2GAME_F6198_cftoe2(const CrtStrFlt* flt, char* buf, int ndigits, int upper, char g)
{
    if (g) crtShift(buf);
    if (flt->sign == '-') *buf++ = '-';
    char* p = buf;
    if (ndigits > 0) { p = buf + 1; *buf = *p; *p = g_decimalPoint; }
    p += ndigits + (g == 0);
    memcpy(p, "e+000", 6);
    if (upper) *p = 'E';
    if (*flt->mantissa != '0') {
        int e = flt->decpt - 1;
        if (e < 0) { e = -e; p[1] = '-'; }
        if (e > 99) { p[2] += (char)(e / 100); e %= 100; }
        if (e > 9) { p[3] += (char)(e / 10); e %= 10; }
        p[4] += (char)e;
    }
}

// 1.13c D2Game+0xF62B6 __cftof2 (in_EAX = flt)
char* D2GAME_F62B6_cftof2(const CrtStrFlt* flt, char* buf, int ndigits, char g)
{
    int dp = flt->decpt;
    if (g && (unsigned)(dp - 1) == (unsigned)ndigits) {
        int neg = flt->sign == '-';
        buf[neg + (dp - 1)] = '0'; buf[neg + (dp - 1) + 1] = 0;
    }
    char* p = buf;
    if (flt->sign == '-') *p++ = '-';
    if (flt->decpt < 1) { crtShift(p); *p++ = '0'; } else p += flt->decpt;
    if (ndigits > 0) {
        crtShift(p); *p = g_decimalPoint;
        if (flt->decpt < 0) {
            int n = -flt->decpt;
            if (g || n <= ndigits) ndigits = n;
            crtShift(p + 1); // retail shifts the tail before the zero fill
            memset(p + 1, '0', ndigits);
        }
    }
    return buf;
}

// Shared helper for F6246/F6352/F63B9: retail calls $I10_OUTPUT (FUN_6fd16ac1) and __fptostr (0xF6990).
// Stand-in: format with snprintf, which produces the identical text for the cases the CRT serves.
// 1.13c D2Game+0xF6246 (%e)
int D2GAME_F6246_cftoe(const double* v, char* buf, int ndigits, int upper)
{ snprintf(buf, 64 + ndigits, upper ? "%.*E" : "%.*e", ndigits, *v); return 0; }

// 1.13c D2Game+0xF6352 (%f)
char* D2GAME_F6352_cftof(const double* v, char* buf, size_t ndigits)
{ snprintf(buf, 400, "%.*f", (int)ndigits, *v); return buf; }

// 1.13c D2Game+0xF63B9 (%g)
void D2GAME_F63B9_cftog(const double* v, char* buf, size_t ndigits, int upper)
{ snprintf(buf, 400, upper ? "%.*G" : "%.*g", (int)(ndigits ? ndigits : 1), *v); }
