// D2Client+0x9A920: left-click on the bottom HUD bar (FUN_6fb4a920, param = mouse-event struct).
// Env-seam style; 32-bit retail layouts, all guest pointers are uint32_t. Ghidra lost EBP/retaddr
// here: EBP is the local player (FUN_6fb47cd0 result), retaddr is param_1.
#include <cstdint>
#include <emscripten.h>
struct D2c9A920Env {
    int32_t* (*f47CD0)();                                   // +0x47CD0 local player
    void (*f16300)(void* ev);                               // +0x16300
    int32_t* mouseX;                                        // DAT_6fbcb828
    int32_t* mouseY;                                        // DAT_6fbcb824
    int32_t* scrW;                                          // DAT_6fb8bc48
    int32_t* scrH;                                          // DAT_6fb8bc4c
    int32_t* panelX;                                        // DAT_6fbcb9a0
    int32_t* panelY;                                        // DAT_6fbcb9a4
    int32_t* g_bef0;                                        // DAT_6fbcbef0
    int32_t* btn;                                           // DAT_6fbcbd80, 5 dwords x N, end 0x6fbcbe5c
    int32_t nbtn;
    int32_t* g_cc3dc; int32_t* g_cc3e0; int32_t* g_cc3f8; int32_t* g_cc3fc;
    int32_t* g_bc28; int32_t* g_bc2c; int32_t* g_bc38;
    int32_t (*ord11017)(int32_t);  int32_t** bbfc;          // DAT_6fbcbbfc
    int32_t (*ord10217)(int32_t h, int32_t i);
    int32_t (*ord10244)(int32_t h, int32_t* u, int a, int b, int c, int d);
    int32_t (*f3C760)(int32_t* u);
    int32_t (*ord11139)(int32_t inv, int32_t i);
    void (*f31CE0)(); void (*f31E40)(); void (*f38A70)(int, int, int, int);
    void (*f4A10)(int); void (*f3F20)(int32_t, int, int);
    void (*ord511)(void* ev);
    int32_t (*ord10227)(); int32_t* g_c9854; int32_t* g_d5d0;
    uint32_t* g_ae10; int32_t* g_ad84; int32_t* g_ad90; int32_t* g_a9e14;
    int32_t* g_cc414; int32_t* g_cc418;
    int32_t (*f55440)(); void (*ord10006)();
    int32_t (*ord10048)(); void (*fECCC0)(int32_t, int); void (*fECC60)(int32_t, int);
    void (*f3BBF0)(); void (*f3A4D0)(int, int, int);
    int32_t* g_c3a68; uint8_t* g_ad80; uint32_t* ptrTbl;    // PTR_DAT_6fb8d538 (+0x90 mode)
    void (*f72790)(int, int, int); void (*fF7B20)(); uint8_t* g_c9721;
    int guestBase;
};
extern "C" {
// 1.13c D2Client+0x9A920
EMSCRIPTEN_KEEPALIVE void d2c_9a920(const D2c9A920Env* e, int32_t* ev) {
    int32_t* pl = e->f47CD0();
    if (!pl) return;
    ev[6] = 0;                                  // +0x18
    e->f16300(ev);
    *e->g_bef0 = 0;
    int32_t mx = *e->mouseX, my = *e->mouseY;
    if (!(mx >= 0 && mx <= *e->scrW / 2 && my >= 0 && my <= *e->scrH - 0x31)) return;
    int32_t px = *e->panelX, py = *e->panelY;
    if (mx < px + 0x110 || px + 0x130 < mx || my < py - 0x5f + *e->scrH || py - 0x3f + *e->scrH < my) {
        int32_t h = e->ord11017((*e->bbfc)[0x18]);
        for (int32_t i = 0; i < e->nbtn; i++) {
            const int32_t* b = e->btn + i * 5;
            if (!(b[0] > 0 && b[0] <= mx && mx <= b[1] && b[2] <= my && my <= b[3])) continue;
            if (h) {
                if (!e->ord10217(h, i) || !e->ord10244(h, pl, 0, 0, 0, 0) || !e->f3C760(pl)) {
                    if (e->ord10217(h, i)) e->f3C760(pl);
                    e->f3F20(pl[3], 1, 0x55);
                } else {
                    e->f31CE0(); e->f31E40(); e->f38A70(0, 0, 0, 0); e->f4A10(0);
                }
                goto done;
            }
            if (pl[0x18] && e->ord11139(pl[0x18], i)) {
                e->f31CE0(); e->f4A10(i);
                *e->g_bc28 = 0; *e->g_bc2c = 0; *e->g_bc38 = 0;
            }
        }
        *e->g_cc3dc = 0; *e->g_cc3e0 = 0; *e->g_cc3f8 = 0; *e->g_cc3fc = 0;
    done:
        ev[6] = 1; ev[7] = 0;
        e->ord511(ev);
        return;
    }
    ev[6] = 1; ev[7] = 0;
    uint32_t old = *e->g_ae10;
    if (!e->ord10227() || !*e->g_c9854) return;
    if (*e->g_d5d0 != 0x26) __builtin_trap();   // retail: message box + _exit(-1)
    uint32_t cur = *e->g_ae10;
    if (!cur) {
        if (*e->g_c3a68) return;
        for (int32_t i = 0; i < *e->g_d5d0; i++) {
            if (e->g_ad80[i] != 1) continue;
            int32_t mode = *(int32_t*)(uintptr_t)(e->guestBase + e->ptrTbl[i] + 0x90);
            if (mode == 1) { e->f72790(i, 1, 0); cur = *e->g_ae10; }
            else if (mode == 4) { if (*e->g_c9721) { e->fF7B20(); cur = *e->g_ae10; } }
            else if (mode != 0) return;
        }
    }
    if (!*e->bbfc || (**e->bbfc == 0 && e->f55440() == 0)) { cur = (cur == 0); *e->g_ae10 = cur; }
    if (old == 0) { if (cur) e->fECCC0(e->ord10048(), 0); }
    else if (!cur) { e->fECC60(e->ord10048(), 0); e->f3BBF0(); }
    int32_t w = *e->scrW, s = *e->g_a9e14, q = (s + ((s >> 31) & 3)) >> 2;
    int32_t a, c;
    if (!*e->g_ad84 && !*e->g_ad90) {
        if (!cur) { *e->g_cc418 = 0; *e->g_cc414 = 0; e->ord10006(); e->f3A4D0(s, 0, 0); return; }
        *e->g_cc418 = (w + ((w >> 31) & 3)) >> 2; *e->g_cc414 = 2; e->ord10006();
        a = s + q; c = q;
    } else if (cur) {
        *e->g_cc418 = 0; *e->g_cc414 = 3; e->ord10006(); e->f3A4D0(s, 0, 0);
        *e->g_cc3dc = 0; *e->g_cc3e0 = 0; *e->g_cc3f8 = 0; *e->g_cc3fc = 0;
        return;
    } else {
        *e->g_cc418 = -((w + ((w >> 31) & 3)) >> 2); *e->g_cc414 = 1; e->ord10006();
        a = s - q; c = -q;
    }
    e->f3A4D0(a, 0, c);
}
}
