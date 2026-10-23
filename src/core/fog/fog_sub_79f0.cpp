// 1.13c Fog+0x079F0 -- CRT math-error dispatcher: builds a _exception record and calls the user matherr hook.
#include <cstdint>

namespace {
struct MathExc { int32_t type; const char* name; double arg1, arg2, retval; };
// Host seams: Fog's matherr hook pointer (0x6FF7E754) and the CRT errno slot (FUN_6ff56a5a).
using MatherrFn = int (*)(MathExc*);
MatherrFn g_matherr = nullptr;
int g_errno = 0;
}

void fog_sub_79f0(const double* p1, const double* p2, double* p3, int op) {
    MathExc e; const char* nm; int32_t ty;
    enum { A, B, C, D } path;                            // A=22h w/ type, B=type1/21h, C=type4 no errno, D=copy then B
    ty = 0; nm = "";
    auto set = [&](int32_t t, const char* n, int p) { ty = t; nm = n; path = (decltype(path))p; };
    if (op > 0xa1) {
        switch (op) {
        case 0xa2: set(4, "exp2", A); break;
        case 0xa6: set(3, "exp10", A); break;
        case 0xaa: set(2, "log2", A); break;
        case 0xab: set(1, "log2", B); break;
        case 1000: set(1, "log", D); break;
        case 0x3e9: set(1, "log10", D); break;
        case 0x3ea: set(1, "exp", D); break;
        case 0x3eb: set(1, "atan", D); break;
        case 0x3ec: set(1, "ceil", D); break;
        case 0x3ed: set(1, "floor", D); break;
        case 0x3ee: set(1, "pow", B); break;
        case 0x3ef: set(1, "modf", D); break;
        default: return;
        }
    } else switch (op) {
        case 0xa1: set(3, "exp2", A); break;
        case 0x18: set(3, "pow", A); break;
        case 2: set(2, "log", A); break;
        case 3: set(1, "log", B); break;
        case 8: set(2, "log10", A); break;
        case 9: set(1, "log10", B); break;
        case 0xe: set(3, "exp", A); break;
        case 0xf: set(4, "exp", C); break;
        case 0x19: set(4, "pow", C); break;
        case 0x1a: *p3 = 1.0; return;
        case 0x1b: set(2, "pow", A); break;
        case 0x1c: set(1, "pow", B); break;
        case 0x1d: set(1, "pow", D); break;
        default: return;
    }
    if (path == D) *p3 = *p1;
    e.type = ty; e.name = nm; e.arg1 = *p1; e.arg2 = *p2; e.retval = *p3;
    int r = g_matherr(&e);
    if (path == A && r == 0) g_errno = 0x22;
    if ((path == B || path == D) && r == 0) g_errno = 0x21;
    *p3 = e.retval;
}
