// 1.13c D2Client+0x7ED30 -- message check against the 128-bucket node table at 0x10A808 (nodes: state dword at +0, key at
// +0xC, next at +0xE4) and the pending keys at 0x113AC2 / 0x113ACA / 0x113AFA / 0x113AFB. Hands the message on through
// 0x7E840 or 0xBE84 or returns a node / key. Stack: message (ret 4). A node whose state is not 1 is fatal in retail
// (0xBF92 / 0xBF80 / 0x3664): the same three calls are made and the walk goes on.
#include "cl_env.h"
namespace {
uint32_t find(const cl::Env& e, uint32_t key) {
    uint32_t n = e.R(0x6FAB0000u + 0x10a808u + 4 * (key & 0x7f));
    while (n && e.R(n + 0xc) != key) n = e.R(n + 0xe4);
    return n;
}
void fatal(const cl::Env& e) {
    const uint32_t v = e.H(0xbf92, 0x5a);
    e.H(0xbf80, v, 0x6FB8359Fu);
    e.H(0x3664, 0xffffffffu);
}
}  // namespace
extern "C" uint32_t d2c_7ed30(uint32_t msg, const cl::Env& e) {
    const uint32_t B = 0x6FAB0000u;
    e.W(B + 0x113aff, 1);
    if (e.H(0x51a80)) {
        if (!e.R(B + 0x113aca)) return e.H(0x7e840, msg);
        uint32_t n = find(e, e.R(B + 0x113ac2));
        if (n && e.R(n) != 1) fatal(e);
        const uint32_t a = e.H(0x51a80);
        if (n == a) return a;
        return e.H(0x7e840, msg);
    }
    if (e.H(0x51a80)) return e.H(0x7e840, msg);
    if (!(e.R(B + 0x113afa) & 0xff)) return e.H(0x7e840, msg);
    const uint32_t k = e.R(B + 0x113afb);
    const uint32_t nk = find(e, k);
    if (!nk) return k;
    if (e.R(nk) != 1) fatal(e);
    const uint32_t nm = find(e, e.R(B + 0x113ac2));
    if (nm && e.R(nm) != 1) fatal(e);
    if (nk != nm) return nm;
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1c, 0);
    return e.H(0xbe84, msg);
}
