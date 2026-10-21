// MSVC 2003 CRT float/validate helpers (D2Game statically linked CRT). Ported from the cached 1.13c decompile.
// Win32 calls (IsBad*Ptr, __controlfp, KERNEL32 lookups) have no wasm equivalent; they are kept as hooks.
#include <cstdint>
#include <cstddef>

namespace d2game_crt_f6458 {

typedef int (*FmtFn)(const uint32_t* arg, char* buf, int a, int b);
// Callees F6246 (%e), F6352 (%f), F63B9 (%g) are not yet ported; set by their owners.
FmtFn g_fmtE = nullptr, g_fmtF = nullptr, g_fmtG = nullptr;

// 1.13c D2Game+0xF6458
int __cfltcvt(double* arg, char* buffer, size_t format, int a, int b, int caps)
{
    (void)caps;
    // Faithful to the decompile: `format` is the sizeInBytes slot holding the conversion char.
    const uint32_t* p = (const uint32_t*)arg;
    if (format == 0x65 || format == 0x45) return g_fmtE ? g_fmtE(p, buffer, a, b) : 0;
    if (format == 0x66) return g_fmtF ? g_fmtF(p, buffer, a, b) : 0;
    return g_fmtG ? g_fmtG(p, buffer, a, b) : 0;
}

unsigned (*g_controlfp)(unsigned, unsigned) = nullptr;
// 1.13c D2Game+0xF64A9
void __setdefaultprecision(void)
{
    if (g_controlfp) g_controlfp(0x10000, 0x30000);
}

// Pentium FDIV probe operands (DAT_6fd1d1a0 / d200 / d208 in .data).
double g_p5_a = 0, g_p5_b = 0, g_p5_c = 0;
// 1.13c D2Game+0xF64BB
uint32_t __ms_p5_test_fdiv(void)
{
    return g_p5_a < g_p5_b - (g_p5_b / g_p5_c) * g_p5_c ? 1 : 0;
}

int (*g_isProcFeature)(unsigned) = nullptr; // KERNEL32!IsProcessorFeaturePresent, if resolved
// 1.13c D2Game+0xF64FB
void __ms_p5_mp_test_fdiv(void)
{
    if (g_isProcFeature) { g_isProcFeature(0); return; }
    __ms_p5_test_fdiv();
}

int (*g_badRead)(const void*, unsigned) = nullptr;
int (*g_badWrite)(void*, unsigned) = nullptr;
int (*g_badCode)(const void*) = nullptr;
int (*g_unhandledFilter)(int*) = nullptr; // DAT_6fd39fb0
void (*g_terminate)(void) = nullptr;

// 1.13c D2Game+0xF65CA
int _ValidateExecute(const void* fn) { return g_badCode ? g_badCode(fn) == 0 : 1; }

// 1.13c D2Game+0xF6524
int sub_F6524(int* ep)
{
    int* rec = (int*)(intptr_t)*ep;
    if (rec[0] == -0x1f928c9d && rec[4] == 3 && (rec[5] == 0x19930520 || rec[5] == 0x19930521) && g_terminate)
        g_terminate();
    if (g_unhandledFilter && _ValidateExecute((const void*)g_unhandledFilter))
        return g_unhandledFilter(ep);
    return 0;
}

// 1.13c D2Game+0xF6592
int _ValidateRead(const void* p, unsigned n) { return g_badRead ? g_badRead(p, n) == 0 : 1; }
// 1.13c D2Game+0xF65AE
int _ValidateWrite(void* p, unsigned n) { return g_badWrite ? g_badWrite(p, n) == 0 : 1; }

// 1.13c D2Game+0xF65E2
uint32_t __ZeroTail(const uint32_t* man, int bit)
{
    int i = bit / 32;
    uint32_t v = man[i] & ~(0xFFFFFFFFu << ((0x1f - (bit % 32)) & 0x1f));
    for (;;) {
        if (v != 0) return 0;
        if (++i > 2) return 1;
        v = man[i];
    }
}

// ___addl(a,b,&out): out=a+b, returns carry
static int addl(uint32_t a, uint32_t b, uint32_t* out) { *out = a + b; return *out < a; }

// 1.13c D2Game+0xF6614
int __IncMan(uint32_t* man, int bit)
{
    uint32_t* p = man + bit / 32;
    int carry = addl(*p, 1u << ((0x1f - (bit % 32)) & 0x1f), p);
    for (int i = bit / 32 - 1; i >= 0; --i) {
        if (!carry) return carry;
        carry = addl(man[i], 1, &man[i]);
    }
    return carry;
}

// 1.13c D2Game+0xF6661
int __RoundMan(uint32_t* man, int bit)
{
    int ret = 0;
    int w = bit / 32;
    unsigned sh = (0x1f - (bit % 32)) & 0x1f;
    if ((man[w] & (1u << sh)) && __ZeroTail(man, bit) == 0)
        ret = __IncMan(man, bit - 1);
    man[w] &= 0xFFFFFFFFu << sh;
    for (int i = w + 1; i < 3; ++i) man[i] = 0;
    return ret;
}

// 1.13c D2Game+0xF6707
void __ShrMan(uint32_t* man, int n)
{
    unsigned s = n % 32;
    uint32_t carry = 0;
    for (int i = 0; i < 3; ++i) {
        uint32_t v = man[i];
        man[i] = (v >> s) | carry;
        carry = (v & ~(0xFFFFFFFFu << s)) << ((0x20 - s) & 0x1f);
    }
    const uint32_t* src = man + (2 - n / 32);
    for (int i = 2; i >= 0; --i, --src)
        man[i] = (i < n / 32) ? 0 : *src;
}

} // namespace d2game_crt_f6458
