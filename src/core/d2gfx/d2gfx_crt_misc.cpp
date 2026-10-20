#include <cstdint>
#include <cstdlib>

// 1.13c D2Gfx+0x042B0 -- MSVC 2003 Release CRT __aulldvrm, the unsigned 64/64 divide used by _output's
// digit loop. Dividend EDX:EAX (hi=param_2, lo=param_1), divisor ECX:EBX (hi=param_4, lo=param_3);
// quotient returned EDX:EAX, remainder ECX:EBX. The decompile's two branches (divisor-high==0 splits the
// quotient into a 64/32 word plus the overflow word; else it normalises the divisor by shifts and
// corrects by one) are the restored divide: both give dividend/divisor and dividend%divisor. A zero
// divisor raises #DE in retail; the wasm i64 divide traps the same way.
// 1.13c D2Gfx+0x042B0
extern "C"
uint64_t d2gfx_aulldvrm_42b0(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi, uint64_t* rem) {
    uint64_t n = ((uint64_t)hi << 32) | lo;
    uint64_t d = ((uint64_t)dhi << 32) | dlo;
    *rem = n % d;
    return n / d;
}

// 1.13c D2Gfx+0x3922 -- MSVC 2003 Release CRT __local_unwind2 (104 B), the compiler's SEH scope-table
// walker. frame+8 is the scope table base, frame+0xC the current trylevel; an entry is 0xC bytes:
// to_state (next index), filter, handler. Walk until the trylevel is -1 or equals target; each step
// stores the entry's to_state back into frame+0xC and, when the entry has no filter, runs the handler.
// Retail brackets the walk with the fs:[0] ExceptionList chain (pushed 12-byte local, restored on exit);
// the caller's frame is not linked by this function, so nothing observable is left to model there.
// The 0x39B6 helper (undecoded, the __NLG_Notify-style thunk) is a notification only; ours does nothing.
struct D2GfxSehEntry { int32_t to_state; int32_t filter; uintptr_t handler; };
extern "C"
void d2gfx_local_unwind2(uintptr_t frame, int32_t target) {
    int32_t* f = (int32_t*)frame;
    int32_t idx;
    while ((idx = f[3]) != -1 && idx != target) {
        D2GfxSehEntry* e = (D2GfxSehEntry*)(uintptr_t)(uint32_t)f[2] + idx;
        f[3] = e->to_state;
        if (e->filter == 0) ((void (*)(void))e->handler)();
    }
}
