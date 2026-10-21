// D2Game batch F4260: MSVC CRT / SEH frame-handler code. Wasm has no SEH, no x86 FS:[0] exception
// chain, no RtlUnwind: those functions are documented stand-ins (state kept in a plain struct
// instead of the per-thread _ptiddata). Implemented from the decompile, no retail content.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>

namespace d2game_f4260 {

// MSVC per-thread exception frame list (_ptiddata::_curexception); single-threaded in wasm.
struct FrameInfo { void* pExceptionObject; FrameInfo* pNext; };
static FrameInfo* g_curException = nullptr;

// 1.13c D2Game+0xF4260 -- percent-scaled stat helper that aborts on bad input (fatal-error path).
// Original takes EAX/EDI implicitly (unaff_EDI = base, in_EAX = limit); here explicit.
// Table lookup (Ordinal_10688 -> +0x19c byte) is a D2Common call, passed as pct by the caller.
uint32_t F4260_scale(uint32_t limit, uint32_t base, uint32_t statValue, uint8_t pct) {
    if (limit < base || pct == 0) std::abort();  // retail: FUN_ error print + _exit(-1)
    return ((uint32_t)pct * (statValue + base)) / 100;
}

// 1.13c D2Game+0xF455A _JumpToContinuation: pops the FS:[0] chain then jumps. No SEH on wasm: stand-in
// just calls the continuation.
void F455A_JumpToContinuation(void (*cont)()) { cont(); }

// 1.13c D2Game+0xF4591 _UnwindNestedFrames: RtlUnwind + clear EXCEPTION_UNWINDING bit (flags & ~2).
// Wasm stand-in: only the flag clear survives.
void F4591_UnwindNestedFrames(uint32_t* exceptionFlags) { *exceptionFlags &= 0xfffffffdu; }

// 1.13c D2Game+0xF45E3 ___CxxFrameHandler: forwards to ___InternalCxxFrameHandler (F5FA1) with
// no translator/target frame. C++ exceptions are not SEH-dispatched on wasm: stand-in returns
// ExceptionContinueSearch (1).
int F45E3_CxxFrameHandler() { return 1; }

// 1.13c D2Game+0xF4654 _CallSETranslator: exception code 0x123 is the "no translator" path
// (sets handler addr, returns 1); otherwise calls the per-thread SE translator. No SE translator
// exists on wasm: returns 0 (not handled).
int F4654_CallSETranslator(uint32_t exceptionCode) { return exceptionCode == 0x123 ? 1 : 0; }

// 1.13c D2Game+0xF47CD _GetRangeOfTrysToCheck.
struct TryBlockMapEntry { int tryLow, tryHigh, catchHigh, nCatches; void* pHandlerArray; };
struct FuncInfo { uint32_t magic; int maxState; void* unwindMap; uint32_t nTryBlocks; TryBlockMapEntry* pTryBlockMap; };
TryBlockMapEntry* F47CD_GetRangeOfTrysToCheck(FuncInfo* fi, int depth, int state, uint32_t* pStart, uint32_t* pEnd) {
    uint32_t n = fi->nTryBlocks, cur = n, end = n;
    TryBlockMapEntry* map = fi->pTryBlockMap;
    while (depth >= 0) {
        uint32_t prev = cur;
        if (n == 0xffffffffu) std::abort();  // _inconsistency
        n--;
        if ((map[n].tryHigh < state && state <= map[n].catchHigh) || n == 0xffffffffu) {
            depth--; cur = n; end = prev;
        } else cur = prev;
    }
    n++;
    *pStart = n; *pEnd = end;
    if (fi->nTryBlocks < end || end < n) std::abort();
    return map + n;
}

// 1.13c D2Game+0xF4847 _CreateFrameInfo: push onto the current-exception list.
FrameInfo* F4847_CreateFrameInfo(FrameInfo* fi, void* obj) {
    fi->pExceptionObject = obj; fi->pNext = g_curException; g_curException = fi; return fi;
}

// 1.13c D2Game+0xF486F IsExceptionObjectToBeDestroyed: 1 unless obj is on the frame list.
int F486F_IsExceptionObjectToBeDestroyed(void* obj) {
    for (FrameInfo* p = g_curException; p; p = p->pNext) if (p->pExceptionObject == obj) return 0;
    return 1;
}

// 1.13c D2Game+0xF4890 _FindAndUnlinkFrame.
void F4890_FindAndUnlinkFrame(FrameInfo* fi) {
    if (fi == g_curException) { g_curException = fi->pNext; return; }
    for (FrameInfo* p = g_curException; p; p = p->pNext)
        if (p->pNext == fi) { p->pNext = fi->pNext; return; }
    std::abort();  // _inconsistency
}

// 1.13c D2Game+0xF48DC _CallCatchBlock2: installs a nested handler frame then __CallSettingFrame
// (F6050) runs the catch funclet. Wasm stand-in: run the funclet, return its continuation.
void* F48DC_CallCatchBlock2(void* (*catchFunclet)()) { return catchFunclet(); }

// 1.13c D2Game+0xF4A57 x87 pow special-case dispatcher (_CIpow tail: switch on FPU status bits
// for NaN/Inf/zero operands, else F4940 core). IEEE semantics of libm pow() cover the same cases.
double F4A57_pow(double x, double y) { return std::pow(x, y); }

// 1.13c D2Game+0xF56CC: fills the __fptrap table (cfltcvt/fassign hooks). Wasm links libc's
// printf float conversion directly; the table does not exist, so nothing to fill.
void F56CC_InitFpTrap() {}

}  // namespace d2game_f4260
