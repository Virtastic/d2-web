// Four register-ABI fragments of the D2Client panel code. Callees through D2cHost (client_host.h), in retail order.
#include "client_host.h"
#include <emscripten.h>

// 1.13c D2Client+0x630F0: fastcall(ecx unused, edx = y) + stack (arg1, arg2), ret 8. Zeroes an 18-word record, rec[0] = arg1,
// rec[13] = DAT_6FBCC084 (g084), then Ordinal 10041 via 0xD096 with (rec, arg2, edx, -1, 5, 0). s[] carries the first four;
// the trailing 5 and 0 are constants of this site.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_630f0(D2cHost h, uint32_t* rec18, uint32_t recTok, uint32_t edx, uint32_t arg1, uint32_t arg2, uint32_t g084) {
    for (int i = 0; i < 18; ++i) rec18[i] = 0;
    rec18[13] = g084;
    rec18[0] = arg1;
    D2cCall c = { 0xD096, 0, 0, 0, 0, 0, { recTok, arg2, edx, 0xFFFFFFFFu } };
    return h(&c);
}

// 1.13c D2Client+0x94CED: tail fragment of the 0x4C8xx walker (the frame's saved registers are popped here). 0x445D0 runs with
// EAX = 0x6FBBB398; then DAT_6FBCBC48 = the byte at frame +0x13, DAT_6FB90EA4 = 1. Returns nothing.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_94ced(D2cHost h, uint8_t frameByte, uint8_t* gBC48, uint32_t* g90EA4) {
    D2cCall c = { 0x945D0, 0x6FBBB398u, 0, 0, 0, 0, { 0, 0, 0, 0 } };
    h(&c);
    *gBC48 = frameByte;
    *g90EA4 = 1;
}

// 1.13c D2Client+0xA53E0: EAX = ptr. Non-zero: 0x54DA0 runs with EBX = ptr and one stack word 0x6FBBA608 (the table base).
// Zero: the retail fatal path, Ordinal 10265(0x961), Ordinal 10024(text 0x6FB8359F, that), _exit(-1) (0x3664); returns 0xFFFFFFFF
// without calling 0x54DA0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a53e0(D2cHost h, uint32_t eax) {
    if (eax == 0) {
        D2cCall a = { 0xBF92, 0, 0, 0, 0, 0, { 0x961, 0, 0, 0 } };
        uint32_t r = h(&a);
        D2cCall b = { 0xBF80, 0, 0, 0, 0, 0, { 0x6FB8359Fu, r, 0, 0 } };
        h(&b);
        D2cCall x = { 0x3664, 0, 0, 0, 0, 0, { 0xFFFFFFFFu, 0, 0, 0 } };
        h(&x);
        return 0xFFFFFFFFu;
    }
    D2cCall c = { 0x54DA0, 0, 0, 0, eax, 0, { 0x6FBBA608u, 0, 0, 0 } };
    return h(&c);
}

// 1.13c D2Client+0xA7CC0: ECX = ecx0, ESI = obj (words). Ordinal 10561 via 0xC5B0 (ecx0, &local) fills local[0], local[1]
// (host out[0..1]). Then 0x57BF0 with ECX = obj[1], EDX = obj[2] + local[0], stack (obj[6] + local[1], obj[5], obj[0]); the
// frame is released by the caller. (The decompile's "local + obj[6]" is a pointer-add misread: the disassembly adds local[1].)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a7cc0(D2cHost h, uint32_t ecx0, const uint32_t* obj, uint32_t localTok) {
    D2cCall a = { 0xC5B0, 0, ecx0, 0, 0, 0, { ecx0, localTok, 0, 0 } };
    h(&a);
    uint32_t l0 = a.out[0], l1 = a.out[1];
    D2cCall b = { 0x57BF0, 0, obj[1], obj[2] + l0, 0, 0, { obj[6] + l1, obj[5], obj[0], 0 } };
    return h(&b);
}
