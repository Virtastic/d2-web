// 1.13c D2Game+0xF6524 (CRT __CxxUnhandledExceptionFilter): stdcall, one argument (EXCEPTION_POINTERS), ret 4.
// A C++ exception record (code 0xE06D7363 at +0, 3 parameters at +0x10, magic 0x19930520 or 0x19930521 at +0x14) calls
// terminate (0x5C47, unconditionally). Then, when the saved previous filter (global 0x119FB0) is set and _ValidateExecute
// (0xF65CA, cdecl, the filter) accepts it, returns the previous filter's result for the same pointers; otherwise 0.
#ifndef D2G_F6524_CPP
#define D2G_F6524_CPP
#include "d2g_mem.h"
namespace d2g_f6524 {
using namespace d2g;
struct Env {
    void (*terminate)();          // 0x5C47
    U prevFilter;                 // the dword at 0x119FB0
    I (*validate)(U fn);          // 0xF65CA (cdecl 1)
    I (*callPrev)(U fn, P ep);    // the indirect call through 0x119FB0 (stdcall 1)
};
// 1.13c D2Game+0xF6524
inline I CxxUnhandledExceptionFilter_113c(const Env& e, P ep) {
    const P rec = pf(ep, 0);
    if (rd(rec, 0) == 0xE06D7363u && rd(rec, 0x10) == 3) {
        const U m = rd(rec, 0x14);
        if (m == 0x19930520u || m == 0x19930521u) e.terminate();
    }
    if (e.prevFilter && e.validate(e.prevFilter)) return e.callPrev(e.prevFilter, ep);
    return 0;
}
}
#endif
