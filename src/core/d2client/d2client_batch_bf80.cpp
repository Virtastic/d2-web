// src/core/d2client/d2client_batch_bf80.cpp -- five UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000; every
// RVA below is retail's. The import veneers were resolved through the local 1.13c PE import table and the
// target DLLs' export tables; the decompiler does not see through a `jmp DWORD PTR ds:[IAT]` thunk and
// renders several of these as self-calls or as functions with unaff_ register arguments. Unrecovered
// callees are host seams passed after the retail arguments (the src/core/d2client/batch pattern).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// Host seams for the two Fog import veneers below.
struct D2ClientBatchBf80Imports {
    // Fog.dll ordinal 10024, `FUN_6ff61100(undefined4, undefined4, undefined4)`: reports the
    // unrecoverable internal error and does not return in retail.
    void (*fogDisplayHalt)(uint32_t a, uint32_t b, uint32_t c);
    // Fog.dll ordinal 10265, `mov eax,[esp]; ret` (RVA 0x0FC20, 4 bytes): returns the address its
    // caller returns to and ignores every stack word.
    uint32_t (*fogCallerReturnAddress)(void);
};

// 1.13c D2Client+0x0BF80 (6 B): import veneer. `6fabbf80: ff 25 48 ee b7 6f  jmp DWORD PTR
// ds:0x6FB7EE48`; IAT slot 0x0CEE48 is Fog.dll ordinal 10024 (FOG_DisplayHalt). The three argument
// words are forwarded unchanged.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_bf80(uint32_t a, uint32_t b, uint32_t c,
                                              const D2ClientBatchBf80Imports* imports) {
    imports->fogDisplayHalt(a, b, c);
}

// 1.13c D2Client+0x0BF92 (6 B): import veneer. `6fabbf92: ff 25 3c ee b7 6f  jmp DWORD PTR
// ds:0x6FB7EE3C`; IAT slot 0x0CEE3C is Fog.dll ordinal 10265 (FOG_CallerReturnAddress). 1.13c's
// implementation at Fog RVA 0x0FC20 is `mov eax,[esp]; ret`: the value is the caller's return
// address and no argument is read (callers that push a code word leave it for their own cleanup).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_bf92(const D2ClientBatchBf80Imports* imports) {
    return imports->fogCallerReturnAddress();
}

// 1.13c D2Client+0x038EC (8 B): `mov esi,[ebp+8]; push esi; call 0x6FAB55E2; pop ecx; ret` -- a
// fragment that forwards the enclosing frame's first stack argument (a FILE*) to __unlock_file.
// The CRT takes the lock away for the whole call, so the host seam owns the unlock.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_38ec(uint32_t stream, void (*unlockFile)(uint32_t)) {
    unlockFile(stream);
}
