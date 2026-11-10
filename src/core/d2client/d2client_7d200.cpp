// 1.13c D2Client+0x7D200 (446 B, EAX = docked flag, stack code (short), confirm; ret 8): enter a game panel layout.
// Docked: 0xE1384 = 0x105 - 0x11B9A4, 0x11EC38 = 0x11B9A0, 0x113AF6 = 1; else 0x11EC38 = (0xDBC48 - 0x145) / 2, 0xE1384 = 0xC,
// 0x113AF6 = 0. Then 0xF46C4/C8 = 0x10, six cursor words cleared, 0x7C9F0, 0x113ACA = 0, 0x7AE00, word 0x113AD2 = code. Without a
// panel object (0x113A68): 0x113A7C = 0; 0xD3A8 (ECX = 8) saves a state that a second 0xD3A8 restores around 0xD1EC (ECX = code,
// stack 2) and 0x7C690 (its result), which builds the object (null: 0xBF92 / 0xBF80 error and _exit). object+0x16 = 0,
// 0x113A80 = 0x113ABA = 1; first time (0x123448): 0x113AEC = now and 0x3CCC0 (EAX = 7, 0xD0B4(), &0xE1388). With 0xDF0D8 set the
// (pointer, code) table at 0xDF398 is searched for code: a nonzero pointer stores 0x38A70(0x11BBFC, 5, 0, 0) at 0x113B0B and
// itself at 0x11BF9C. Codes 0x7F, 0x4E22, 0x4EC9, 0x4EA3, 0x18C with confirm set 0x113AF0 = 1.
#include <initializer_list>
#include "cl_env.h"
extern "C" void d2c_7d200(uint32_t docked, uint32_t code, uint32_t confirm, uint32_t now, const cl::Env& e) {
    constexpr uint32_t B = 0x6FAB0000u;
    if (docked) {
        e.W(B + 0xE1384, 0x105u - e.R(B + 0x11B9A4)); e.W(B + 0x11EC38, e.R(B + 0x11B9A0)); e.W(B + 0x113AF6, 1);
    } else {
        e.W(B + 0x11EC38, (uint32_t)(((int32_t)e.R(B + 0xDBC48) - 0x145) / 2)); e.W(B + 0x113AF6, 0); e.W(B + 0xE1384, 0xc);
    }
    e.W(B + 0xF46C8, 0x10); e.W(B + 0xF46C4, 0x10);
    for (uint32_t a : { 0x11C3DCu, 0x11C3E0u, 0x11C3F8u, 0x11C3FCu, 0x11C2F8u, 0x11C2F4u }) e.W(B + a, 0);
    e.H(0x7c9f0);
    e.W(B + 0x113ACA, 0);
    e.H(0x7ae00);
    e.W(B + 0x113AD2, (e.R(B + 0x113AD2) & 0xffff0000u) | (code & 0xffff));
    if (e.R(B + 0x113A68)) return;
    e.W(B + 0x113A7C, 0);
    const uint32_t saved = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 8);
    const uint32_t str = e.H(0xd1ec, 2, 0, 0, 0, 0, 0, code);
    const uint32_t p = e.H(0x7c690, str);
    e.W(B + 0x113A68, p);
    if (!p) {   // retail does not return from _exit
        const uint32_t msg = e.H(0xbf92, 0x1051);
        e.H(0xbf80, 0xB20D359Fu, msg);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, saved);
    e.W(p + 0x16, 0); e.W(B + 0x113A80, 1); e.W(B + 0x113ABA, 1);
    if (!e.R(B + 0x123448)) {
        e.W(B + 0x123448, 1);
        e.W(B + 0x113AEC, now);
        const uint32_t v = e.H(0xd0b4);
        e.H(0x3ccc0, v, 0xB20E1388u, 0, 0, 0, 7);
    }
    if (e.R(B + 0xDF0D8)) {
        for (uint32_t a = B + 0xDF398; e.R(a); a += 8) {
            if ((e.R(a + 4) & 0xffff) != (code & 0xffff)) continue;
            const uint32_t x = e.R(a);
            if (x) { e.W(B + 0x113B0B, e.H(0x88a70, e.R(B + 0x11BBFC), 5, 0, 0)); e.W(B + 0x11BF9C, x); }
            break;
        }
    }
    const uint32_t c = code & 0xffff;
    if ((c == 0x7f || c == 0x4e22 || c == 0x4ec9 || c == 0x4ea3 || c == 0x18c) && confirm) e.W(B + 0x113AF0, 1);
}
