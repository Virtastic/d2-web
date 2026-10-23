// src/core/d2gfx/d2gfx_batch_b65.cpp -- D2Gfx MSVC2003 C++-EH runtime helpers, reimplemented from the cached
// decompiles. Alpha batch lane: implementation + marker only. The per-thread data block (__getptd) is an
// explicit leading param; hooks the decompile reaches through globals/NLG are explicit callbacks.
// Skipped: 0xBF40 _CallCatchBlock2 (SEH frame + __CallSettingFrame), 0xC28D (mid-function funclet, unaff_EBP/ESI/EDI),
// 0xC2FC BuildCatchObject (SEH, ECX/EDX/ESI/EDI register inputs), 0xC4DF, 0xC59D (rely on unrecovered register state).
#include <cstdint>
#include <cstdlib>
#include <initializer_list>

struct D2GfxPtd { void* curexception; void* purecall_cnt_unused; void* unexpected; void* translator; void (*terminate_fn)(); };
struct D2GfxFrameInfo { void* pExc; D2GfxFrameInfo* next; };  // x86 +0, +4
struct D2GfxStateEntry { int32_t toState; void* action; };    // _s_UnwindMapEntry, 8 B on x86

// 1.13c D2Gfx+0x0C843 (terminate): call per-thread terminate handler if set, then abort.
extern "C" [[noreturn]] void d2gfx_sub_C843(D2GfxPtd* ptd) {
    if (ptd->terminate_fn) ptd->terminate_fn();
    std::abort();
}

// 1.13c D2Gfx+0x0C878 (_inconsistency): optional global hook (PTR_terminate_6fa90d90), then terminate.
extern "C" [[noreturn]] void d2gfx_sub_C878(D2GfxPtd* ptd, void (*hook)()) {
    if (hook) hook();
    d2gfx_sub_C843(ptd);
}

// 1.13c D2Gfx+0x0BEF4 (_FindAndUnlinkFrame): unlink frame from ptd's singly linked list; absent -> _inconsistency.
extern "C" void d2gfx_sub_BEF4(D2GfxPtd* ptd, D2GfxFrameInfo* frame, void (*hook)()) {
    if (frame == (D2GfxFrameInfo*)ptd->curexception) { ptd->curexception = frame->next; return; }
    D2GfxFrameInfo* p = (D2GfxFrameInfo*)ptd->curexception;
    for (;;) {
        if (!p->next) d2gfx_sub_C878(ptd, hook);
        if (p->next == frame) break;
        p = p->next;
    }
    p->next = frame->next;
}

// 1.13c D2Gfx+0x0C0D4 (___DestructExceptionObject): if obj and its throw-info dtor (throwInfo+4) exist, call dtor on object.
// obj layout (x86): +0x18 pExceptionObject, +0x1c pThrowInfo.
struct D2GfxExcRec { uint8_t pad[0x18]; void* pObj; struct { void* pad; void (*dtor)(void*); }* pThrow; };
extern "C" void d2gfx_sub_C0D4(D2GfxExcRec* obj) {
    if (obj && obj->pThrow && obj->pThrow->dtor) obj->pThrow->dtor(obj->pObj);
}

// 1.13c D2Gfx+0x0C8B0 (__CallSettingFrame@12): notify(code), call handler, notify(code==0x100 ? 2 : code).
extern "C" void d2gfx_sub_C8B0(void (*handler)(), int code, void (*notify)(int)) {
    notify(code);
    handler();
    notify(code == 0x100 ? 2 : code);
}

// 1.13c D2Gfx+0x0C006 (___FrameUnwindToState): unwind frame state (frame+8) down to target via the unwind map
// (funcInfo+4 count, +8 table); each entry with an action runs it through __CallSettingFrame (callSetting, code 0x103).
// ptd->purecall_cnt_unused is the try-level counter the original bumps (+1); epilogue helper 0xC0B9 is `epilogue`.
extern "C" void d2gfx_sub_C006(D2GfxPtd* ptd, int32_t* frameState, int32_t maxState, const D2GfxStateEntry* map,
                               int32_t target, void (*hook)(), void (*callSetting)(void*, int32_t*, int),
                               void (*epilogue)()) {
    int32_t s = *frameState;
    ptd->purecall_cnt_unused = (void*)((intptr_t)ptd->purecall_cnt_unused + 1);
    while (s != target) {
        if (s < 0 || maxState <= s) d2gfx_sub_C878(ptd, hook);
        const D2GfxStateEntry& e = map[s];
        s = e.toState;
        if (e.action) { *frameState = s; callSetting(e.action, frameState, 0x103); }
    }
    epilogue();
    *frameState = s;
}

// 1.13c D2Gfx+0x0BF40 _CallCatchBlock2: pushes a nested SEH frame (handler LAB_6fa8bc7d, state param_4+1) then
// returns __CallSettingFrame_12(handler, regNode, flags). The SEH frame has no wasm analogue; the result is the
// catch funclet's continuation address.
void* d2gfx_call_catch_block2(void* (*catchFunclet)()) { return catchFunclet(); }
