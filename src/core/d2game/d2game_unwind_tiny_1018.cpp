// src/core/d2game/d2game_unwind_tiny_1018.cpp -- seven 1.13c D2Game.dll leaves transcribed from
// the cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: six VC7.1 SEH unwind
// funclets of the game-table construction/destruction scopes
// and the CRT _CallMemberFunction1 tail-jump helper. Alpha batch lane (OBJECTIVE v8.3): implementation +
// marker only, no spec, no vector.
//
// Host model: each funclet receives the EBP of the frame being unwound (the d2client_batch_crt_unwind_a4ac
// pattern), reads its operands by retail EBP offset and tail-jumps into the callee. The list helpers are the
// ones already modelled in gametbl_hash32.cpp: 0x1FE0 listUninit and 0x1FA0 unlinkLink (the terminator link
// unlink, de-static'd there for reuse); a frame address handed to them is truncated to the 32-bit guest
// address space those helpers use. The member-function call in 0xF458A is a seam: it is a stack-layout
// tail jump, not callable from a C signature.
#include <cstdint>
#include <emscripten.h>

// 0x1FE0 listUninit and 0x1FA0 unlinkLink (src/core/d2game/gametbl_hash32.cpp).
void listUninit(uint32_t list);
void unlinkLink(uint32_t link);

static inline uint32_t guestAddr(const uint32_t* p) { return (uint32_t)(uintptr_t)p; }

// 1.13c D2Game+0x0F7810 (11 B): mov 0x4(%ebp),%eax; add $4,%eax; jmp 0x1FA0 -- unwind fragment that
// unlinks the terminator TSLink at (EBP+4)+4, i.e. list+4. Tail call, no return value.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f7810(const uint32_t* ebp) {
    unlinkLink(ebp[1] + 4);
}

// 1.13c D2Game+0x0F7830 (10 B): push 0x4(%ebp); call 0x1FE0; ret -- unwind the list at EBP+4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f7830(const uint32_t* ebp) {
    listUninit(ebp[1]);
}

// 1.13c D2Game+0x0F7850 (11 B): identical body to 0x0F7810 -- terminator unlink of (EBP+4)+4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f7850(const uint32_t* ebp) {
    unlinkLink(ebp[1] + 4);
}

// 1.13c D2Game+0x0F785B (10 B): identical body to 0x0F7830 -- unwind the list at EBP+4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f785b(const uint32_t* ebp) {
    listUninit(ebp[1]);
}

// 1.13c D2Game+0x0F7880 (10 B): lea -0x18(%ebp),%eax; push eax; call 0x1F60; ret -- 0x1F60 is the
// ExceptionList-protected wrapper of 0x1FE0, so the host effect is listUninit of the frame list at EBP-0x18.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f7880(const uint32_t* ebp) {
    listUninit(guestAddr(ebp - 6));
}

// 1.13c D2Game+0x0F788A (10 B): lea -0x18(%ebp),%eax; push eax; call 0x1FE0; ret -- same list, direct call.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f788a(const uint32_t* ebp) {
    listUninit(guestAddr(ebp - 6));
}

// 0xF458A (_CallMemberFunction1) seam: pfn is called with pthis in ECX and arg as its first stack argument.
struct D2GameTinyUnwindEnv {
    void (*callMember)(void* pfn, void* pthis, uint32_t arg);
};

// 1.13c D2Game+0x0F458A (7 B): pop eax; pop ecx; xchg eax,(esp); jmp *eax. The helper discards its own
// return address, takes ECX = pthis from the first stack slot, exchanges the member-function address into
// the return-address slot and tail-jumps to it, so the member function returns straight to the caller's
// continuation with pval in its argument slot. The stack swap cannot ride a C signature; it is the seam.
// Retail prototype is __stdcall _CallMemberFunction1(void *, void *, void *).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f458a(void* pthis, void* pfn, uint32_t arg,
                                              const D2GameTinyUnwindEnv* env) {
    if (env != nullptr && env->callMember != nullptr) env->callMember(pfn, pthis, arg);
}
