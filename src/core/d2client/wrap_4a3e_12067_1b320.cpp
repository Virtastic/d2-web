// Three thin D2Client wrappers whose only work is to marshal a record into a callee. Callees go through D2cHost
// in retail's call order (client_host.h). A hidden EBP register argument rides in D2cCall::s[3].
#include "client_host.h"
#include <emscripten.h>

// 1.13c D2Client+0x4A3E: stdcall(p) with one stack word, ret 4. EBP = p[0]; calls 0x915E(p[6], p[7]) cdecl
// (arguments are the words at p+0x18 and p+0x1C). Returns the callee's eax.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_4a3e(D2cHost h, const uint32_t* p) {
    D2cCall c = { 0x915E, 0, 0, 0, 0, 0, { p[6], p[7], 0, p[0] } };
    return h(&c);
}

// 1.13c D2Client+0x12067: stdcall(rec), 0x119C0 gets EAX = 0x100; rec[6] = 1, rec[7] = 0 (words at +0x18, +0x1C);
// then tail-jumps to the thunk 0xBE84 (Ordinal 511) with the same single stack word, the record pointer.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_12067(D2cHost h, uint32_t* rec, uint32_t recTok) {
    D2cCall a = { 0x119C0, 0x100, 0, 0, 0, 0, { 0, 0, 0, 0 } };
    h(&a);
    rec[6] = 1;
    rec[7] = 0;
    D2cCall b = { 0xBE84, 0, 0, 0, 0, 0, { recTok, 0, 0, 0 } };
    return h(&b);
}

// 1.13c D2Client+0x1B320: EAX = arg (a pointer or 0), ECX is overwritten. Builds a stack record of 8 words
// { +4 local player (DAT 0x6FBCBBFC), +8 arg, +0x18 2 } and calls 0x19EF0 (ours d2_client_19ef0) with ECX = &record and
// EAX = (arg == 0 ? -1 : *(arg + 0xC)). The two arms differ only in that EAX; the record is the same.
// rec: caller-owned 8 words (zeroed here), recTok its guest address, argWord3 = the word at arg + 0xC (read only when arg != 0).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1b320(D2cHost h, uint32_t* rec, uint32_t recTok, uint32_t player, uint32_t arg, uint32_t argWord3) {
    for (int i = 0; i < 8; ++i) rec[i] = 0;
    rec[1] = player;
    rec[2] = arg;
    rec[6] = 2;
    D2cCall c = { 0x19EF0, arg ? argWord3 : 0xFFFFFFFFu, recTok, 0, 0, 0, { 0, 0, 0, 0 } };
    h(&c);
}
