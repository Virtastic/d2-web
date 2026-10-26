// Fog MSVC CRT / C++ EH runtime helpers, batch z15. Retail globals/callees are seam params;
// 32-bit pointers are uint32_t links.
#include <cstdint>

// 1.13c Fog+0x214F0  __alldiv (signed 64/64)
int64_t alldiv_z15(int64_t a, int64_t b) {
    bool neg = false;
    uint64_t ua = (uint64_t)a, ub = (uint64_t)b;
    if (a < 0) { ua = 0 - ua; neg = !neg; }
    if (b < 0) { ub = 0 - ub; neg = !neg; }
    uint64_t q = ua / ub;
    return (int64_t)(neg ? 0 - q : q);
}

// 1.13c Fog+0x216FF  _JumpToContinuation: pops ExceptionList then jumps to target
void JumpToContinuation_z15(uint32_t* exceptionList, uint32_t target, void (*jump)(uint32_t)) {
    // ExceptionList = *ExceptionList (caller passes the head's next via seam read)
    *exceptionList = *(uint32_t*)(uintptr_t)*exceptionList;
    jump(target);
}

// 1.13c Fog+0x2172F  _CallMemberFunction1: tail-jump to param_2 (thiscall shim)
void CallMemberFunction1_z15(uint32_t arg, uint32_t fn, void (*callFn)(uint32_t fn, uint32_t arg)) {
    callFn(fn, arg);
}

// 1.13c Fog+0x21736  _UnwindNestedFrames
void UnwindNestedFrames_z15(uint32_t reg, uint8_t* excRec, uint32_t* exceptionList,
                            void (*rtlUnwind)(uint32_t reg, uint32_t target, uint8_t* rec)) {
    uint32_t saved = *exceptionList;
    rtlUnwind(reg, 0x6ff7175f, excRec);
    *(uint32_t*)(excRec + 4) &= 0xfffffffdu;
    // *(void**)saved = ExceptionList; ExceptionList = saved
    *(uint32_t*)(uintptr_t)saved = *exceptionList;
    *exceptionList = saved;
}

// 1.13c Fog+0x21788  ___CxxFrameHandler: forwards to ___InternalCxxFrameHandler (Fog+0x23302)
typedef uint32_t (*InternalCxxFrameHandler_t)(uint32_t rec, uint32_t reg, uint32_t ctx, uint32_t disp,
                                              uint32_t funcInfo, int recursive, uint32_t target, uint8_t unwind);
uint32_t CxxFrameHandler_z15(uint32_t rec, uint32_t reg, uint32_t ctx, uint32_t disp, uint32_t funcInfoEax,
                             InternalCxxFrameHandler_t internal) {
    return internal(rec, reg, ctx, disp, funcInfoEax, 0, 0, 0);
}

// 1.13c Fog+0x217F9  _CallSETranslator
// ptd seam: nlgCode(code, &rec) is _ptiddata::_NLG_dwCode; guard frame links elided (SEH is not modelled).
int CallSETranslator_z15(uint32_t rec, uint32_t reg, void (*nlgCode)(uint32_t code, uint32_t* recPtr),
                         uint32_t recCode, uint32_t param3, uint32_t* regFirstDword, uint32_t unwindHandler) {
    if (rec == 0x123) { *regFirstDword = unwindHandler; (void)reg; return 1; }
    uint32_t local[2] = { rec, param3 };
    nlgCode(recCode, local);
    return 0;
}

// 1.13c Fog+0x21972  _GetRangeOfTrysToCheck
struct TryBlockMapEntry_z15 { int tryLow, tryHigh, catchHigh, nCatches; uint32_t pHandlerArray; };
struct FuncInfo_z15 { uint32_t magic; int maxState; uint32_t pUnwindMap; uint32_t nTryBlocks; uint32_t pTryBlockMap; };
TryBlockMapEntry_z15* GetRangeOfTrysToCheck_z15(const FuncInfo_z15* fi, int a2, int a3, uint32_t* lo, uint32_t* hi,
                                                TryBlockMapEntry_z15* tryMap, void (*inconsistency)()) {
    uint32_t n = fi->nTryBlocks, u1 = n, u4 = n, u3;
    while (u3 = u1, a2 >= 0) {
        if (n == 0xffffffffu) inconsistency();
        n--;
        if ((tryMap[n].tryHigh < a3 && a3 <= tryMap[n].catchHigh) || (u1 = u3, n == 0xffffffffu)) {
            a2--; u1 = n; u4 = u3;
        }
    }
    n++;
    *lo = n; *hi = u4;
    if (fi->nTryBlocks < u4 || u4 < n) inconsistency();
    return tryMap + n;
}

// FrameInfo = { uint32 object; uint32 next } linked through ptd->_curexception (seam: *curexc, 32-bit links).
// 1.13c Fog+0x219EC  _CreateFrameInfo
uint32_t CreateFrameInfo_z15(uint32_t fi, uint32_t obj, uint32_t* curexc, uint32_t* (*mem)(uint32_t)) {
    mem(fi)[0] = obj;
    mem(fi)[1] = *curexc;
    *curexc = fi;
    return fi;
}

// 1.13c Fog+0x21A14  IsExceptionObjectToBeDestroyed
int IsExceptionObjectToBeDestroyed_z15(uint32_t obj, uint32_t curexc, uint32_t* (*mem)(uint32_t)) {
    for (uint32_t p = curexc; p; p = mem(p)[1])
        if (mem(p)[0] == obj) return 0;
    return 1;
}

// 1.13c Fog+0x21A35  _FindAndUnlinkFrame
void FindAndUnlinkFrame_z15(uint32_t fi, uint32_t* curexc, uint32_t* (*mem)(uint32_t), void (*inconsistency)()) {
    if (fi == *curexc) { *curexc = mem(fi)[1]; return; }
    uint32_t p = *curexc;
    for (;;) {
        uint32_t nx = mem(p)[1];
        if (nx == 0) { inconsistency(); return; }
        if (nx == fi) break;
        p = nx;
    }
    mem(p)[1] = mem(fi)[1];
}

// 1.13c Fog+0x21A81  _CallCatchBlock2
// Returns __CallSettingFrame_12(handler, reg, code) (Fog+0x233B0); the guard frame (local_8 = state+1,
// funcInfo, reg, ExceptionList push/pop) is SEH bookkeeping owned by the seam.
uint32_t CallCatchBlock2_z15(uint32_t reg, uint32_t funcInfo, uint32_t handler, int state, uint32_t code,
                             uint32_t (*callSettingFrame12)(uint32_t handler, uint32_t reg, uint32_t code)) {
    (void)funcInfo; (void)state;
    return callSettingFrame12(handler, reg, code);
}

// 1.13c Fog+0x22B67  ___FrameUnwindToState
// reg+8 = current state; unwind map entries are {int toState; uint32 action}; ptd->_purecall counts unwinds.
// Final FUN_6ff72c1a (Fog+0x22C1A) is a seam call, arguments unrecovered (guess: none).
void FrameUnwindToState_z15(uint8_t* reg, int funcInfoMaxState, const uint32_t* unwindMap /*pairs*/, int target,
                            uint32_t* purecall, void (*callSettingFrame12)(uint32_t action, uint8_t* reg, uint32_t code),
                            void (*fn22C1A)(), void (*inconsistency)()) {
    int st = *(int*)(reg + 8);
    ++*purecall;
    while (st != target) {
        if (st < 0 || funcInfoMaxState <= st) inconsistency();
        const uint32_t* e = unwindMap + st * 2;
        st = (int)e[0];
        if (e[1] != 0) { *(int*)(reg + 8) = st; callSettingFrame12(e[1], reg, 0x103); }
    }
    fn22C1A();
    if (st != target) inconsistency();
    *(int*)(reg + 8) = st;
}
