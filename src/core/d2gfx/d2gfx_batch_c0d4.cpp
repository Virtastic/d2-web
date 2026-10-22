// D2Gfx VC7.1 CRT signal/EH leaves, read against the cached 1.13c decompiles (alpha batch lane: no spec, no vector).
// Seams follow d2gfx_getptd.cpp: callees and module globals are caller-supplied.
// Skipped, not faithful on wasm: BF40 _CallCatchBlock2 (builds an FS:[0] frame), C28D (reads the caller's EBP frame).
#include <cstdint>

// 1.13c D2Gfx+0x0C0D4 ___DestructExceptionObject: if the object is non-null and its throw info's
// destructor (throwinfo+4, throwinfo = obj+0x1C) is non-null, run it on the object pointer at obj+0x18.
extern "C"
void d2gfx_destruct_exception_object(uint8_t* mem, uint32_t obj,
                                     void (*call_member_function1)(uint32_t thisp, uint32_t fn)) {
    if (obj == 0) return;
    uint32_t dtor = *(uint32_t*)(mem + *(uint32_t*)(mem + obj + 0x1C) + 4);
    if (dtor != 0) call_member_function1(*(uint32_t*)(mem + obj + 0x18), dtor);
}

// 1.13c D2Gfx+0x0C8B0 __CallSettingFrame@12: NLG_Notify1(code) returns the target to call, call it,
// then NLG_Notify1(code==0x100 ? 2 : code).
extern "C"
void d2gfx_call_setting_frame(uint32_t code, uint32_t (*nlg_notify1)(uint32_t),
                              void (*call)(uint32_t target)) {
    call(nlg_notify1(code));
    nlg_notify1(code == 0x100 ? 2 : code);
}

// 1.13c D2Gfx+0x0C9D2 siglookup: entries are 12 bytes with the signal number at +4. Returns the entry
// whose number matches, or 0 once the scan passes table + count*12 (count = DAT_6FA90224).
extern "C"
uint8_t* d2gfx_siglookup(uint8_t* table, uint32_t count, int32_t signum) {
    for (uint8_t* p = table; p < table + count * 12; p += 12)
        if (*(int32_t*)(p + 4) == signum) return p;
    return nullptr;
}

// 1.13c D2Gfx+0x0C006 ___FrameUnwindToState: walk the unwind map (entries of 8: toState, action) from the
// frame's state (frame+8) down to targetState, counting _purecall up, setting the frame state
// and running each action through __CallSettingFrame(action, frame, 0x103). Bad index -> _inconsistency.
struct D2GfxUnwindHost {
    void (*call_setting_frame)(uint32_t action, uint32_t frame, uint32_t code);
    void (*inconsistency)(void);
    uint32_t* purecall;   // __getptd()->_purecall
    void (*sub_c0b9)(void);
};
extern "C"
void d2gfx_frame_unwind_to_state(uint8_t* mem, const D2GfxUnwindHost* h, uint32_t frame,
                                 uint32_t funcinfo, int32_t target) {
    int32_t state = *(int32_t*)(mem + frame + 8);
    *h->purecall += 1;
    while (state != target) {
        if (state < 0 || *(int32_t*)(mem + funcinfo + 4) <= state) h->inconsistency();
        uint8_t* e = mem + *(uint32_t*)(mem + funcinfo + 8) + state * 8;
        state = *(int32_t*)e;
        if (*(uint32_t*)(e + 4) != 0) {
            *(int32_t*)(mem + frame + 8) = state;
            h->call_setting_frame(*(uint32_t*)(e + 4), frame, 0x103);
        }
    }
    h->sub_c0b9();
    if (state != target) h->inconsistency();
    *(int32_t*)(mem + frame + 8) = state;
}

// 1.13c D2Gfx+0x0CA00 _raise. ptd+0x54 = action table, +0x58 saved siginfo ptr, +0x5C fpe code.
// Note: the exit(3) on a null handler is terminal in retail; on a returning host the lock below still runs.
struct D2GfxRaiseHost {
    uint8_t* ptd;                    // __getptd()
    uint32_t* sig_int; uint32_t* sig_term; uint32_t* sig_break; uint32_t* sig_abort; // 9D68C, 9D698, 9D690, 9D694
    uint32_t fpe_first, fpe_num, siglookup_count;   // DAT_90218, DAT_9021C, DAT_90224
    void (*lock)(int32_t);           // __lock
    void (*unlock)(void);            // 0xCB3B
    void (*exit_)(int32_t);          // __exit
    void (*call_handler)(uint32_t fn, int32_t sig, uint32_t extra, bool two_args);
};
extern "C"
int32_t d2gfx_raise(const D2GfxRaiseHost* h, int32_t sig) {
    uint8_t* ptd = h->ptd;
    uint32_t* slot; uint32_t fn; uint32_t global = 0;
    if (sig == 2) { slot = h->sig_int; global = 1; }
    else if (sig == 15) { slot = h->sig_term; global = 1; }
    else if (sig == 21) { slot = h->sig_break; global = 1; }
    else if (sig == 22) { slot = h->sig_abort; global = 1; }
    else if (sig == 4 || sig == 8 || sig == 11) {
        uint8_t* e = d2gfx_siglookup(*(uint8_t**)(ptd + 0x54), h->siglookup_count, sig);
        slot = (uint32_t*)(e + 8);
    } else return -1;
    fn = *slot;
    if (fn == 1) return 0;
    if (fn == 0) h->exit_(3);
    if (global) h->lock(0);   // only the four process-wide slots take the signal lock
    uint32_t oldinfo = 0, oldfpe = 0;
    bool ptdsig = sig == 8 || sig == 11 || sig == 4;
    if (ptdsig) {
        oldinfo = *(uint32_t*)(ptd + 0x58); *(uint32_t*)(ptd + 0x58) = 0;
        if (sig == 8) { oldfpe = *(uint32_t*)(ptd + 0x5C); *(uint32_t*)(ptd + 0x5C) = 0x8C; }
    }
    if (sig == 8) {
        uint8_t* tab = *(uint8_t**)(ptd + 0x54);
        for (uint32_t i = h->fpe_first; i < h->fpe_num + h->fpe_first; i++) *(uint32_t*)(tab + i * 12 + 8) = 0;
    } else *slot = 0;
    h->unlock();
    if (sig == 8) h->call_handler(fn, 8, *(uint32_t*)(ptd + 0x5C), true);
    else {
        h->call_handler(fn, sig, 0, false);
        if (sig != 11 && sig != 4) return 0;
    }
    *(uint32_t*)(ptd + 0x58) = oldinfo;
    if (sig == 8) *(uint32_t*)(ptd + 0x5C) = oldfpe;
    return 0;
}
