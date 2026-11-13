// 1.13c D2Client+0xA4860 -- left-button listener of the screen context (stdcall msg, ret 4). The context is the record at [0x11BC00] (none: the
// 0x15B assertion). A press less than 0x50 ticks after the context's stamp ([ctx]+0x50 against GetTickCount, unsigned) is only finished
// (+0x18 = 1, +0x1C = 0, shared tail 0xBE84). Otherwise the listener chain at ctx+0xB08 is walked (node: +0 listener object, +4 next; each
// listener's vtable slot +0xC is called with the message, ECX = the object; an answer of 1 finishes the message). Then ctx+0x10 := 1 and
// 0xA44F0 (EAX = ctx, stack = msg) runs; nonzero finishes. Else the press (x = msg+0xC word, y = msg+0xE word) inside the box
// [ctx+0x2C - 4 .. +0x2C + [ctx+0x34] + 4] x [ctx+0x30 - 4 .. +0x30 + [ctx+0x38] + 4] finishes too. Outside: with [0xFADD4] set 0x62F20 (ECX = x) nonzero
// returns; then the callback ctx+0x64 (answer zero returns) is run, 0x4FF60 (EAX = [0x11C414], ECX = x) when [0xFAD98] is set finishes the message
// when nonzero, and with [0xFAD9C] set 0x4FEC0 (EDI = y) nonzero finishes it and zero returns; without a +0x64 callback ctx+0x60 is called and the
// message finished (neither: the 0x1AA assertion).
#include "cl_pa4.h"
extern "C" uint32_t d2c_a4860(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t p = G(e, 0x11bc00);
    if (!p) { assertAt(e, 0x15b); return 0; }
    const uint32_t tk = e.H(0xcef5c);
    if (tk < e.R(p) + 0x50u) return fin(e, msg);
    for (uint32_t n = e.R(p + 0xb08); n; n = e.R(n + 4)) {
        const uint32_t obj = e.R(n);
        if (obj && e.H(e.R(e.R(obj) + 0xc) & 0xFFFFFFu, msg, 0, 0, 0, 0, 0, obj) == 1) return fin(e, msg);
    }
    e.W(p + 0x10, 1);
    const uint32_t r = e.H(0xa44f0, msg, 0, 0, 0, 0, p);
    e.Ej(0xa44f0, p);
    if (r) return fin(e, msg);
    const uint32_t x = P16(e, msg + 0xc), y = P16(e, msg + 0xe);
    const uint32_t bx = e.R(p + 0x2c), by = e.R(p + 0x30);
    if (!((int32_t)x < (int32_t)(bx - 4) || (int32_t)x > (int32_t)(e.R(p + 0x34) + bx + 4) || (int32_t)y < (int32_t)(by - 4) ||
          (int32_t)y > (int32_t)(e.R(p + 0x38) + by + 4)))
        return fin(e, msg);
    if (G(e, 0xfadd4)) {
        const uint32_t q = e.H(0x62f20, 0, 0, 0, 0, 0, G(e, 0xfadd4), x);
        e.Ej(0x62f20, p);
        if (q) return 0;
    }
    const uint32_t cb = e.R(p + 0x64);
    if (cb) {
        if (e.H(cb & 0xFFFFFFu) == 0) return 0;
        if (G(e, 0xfad98)) {
            const uint32_t q = e.H(0x4ff60, 0, 0, 0, 0, 0, G(e, 0x11c414), x);
            e.Ej(0x4ff60, p);
            if (q) fin(e, msg);
        }
        if (!G(e, 0xfad9c)) return 0;
        const uint32_t q = e.H(0x4fec0, 0, 0, 0, 0, 0, G(e, 0x11c414), x);
        e.Ej(0x4fec0, y);
        if (!q) return 0;
        return fin(e, msg);
    }
    const uint32_t c2 = e.R(p + 0x60);
    if (!c2) { assertAt(e, 0x1aa); return 0; }
    e.H(c2 & 0xFFFFFFu);
    return fin(e, msg);
}
