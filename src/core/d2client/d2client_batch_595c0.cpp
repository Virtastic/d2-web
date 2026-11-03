// src/core/d2client/d2client_batch_595c0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x595C0..0x59BC0, the
// exit/disconnect-confirm state machine) reimplemented from the cached decompiles. Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Base 0x6FAB0000.
// Module state is one struct (retail RVA beside each field); unrecovered callees are host seams after the
// retail arguments (the d2client_batch_0312a.cpp pattern). Callback arguments are retail RVAs.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

#include "d2client_595c0.h"

static void sendO(const D2cExitEnv* e, uint8_t op) {
    uint8_t p[8] = {'O', op, 0, 0, 0, 0, 0, 0};
    e->send(p);
}

// 1.13c D2Client+0x595C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_595c0(D2cExitState* s, const D2cExitEnv* e) {
    sendO(e, 7);
    s->state = 3;
}

// 1.13c D2Client+0x59600
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_59600(D2cExitState* s, const D2cExitEnv* e) {
    s->state = 7;
    uint8_t p[8] = {'O', 4, 0, (uint8_t)s->val77ee, (uint8_t)(s->val77ee >> 8),
                    (uint8_t)s->val77ec, (uint8_t)(s->val77ec >> 8), 0};
    e->send(p);
    s->tick = 0xFFFFFFFFu;
}

// 1.13c D2Client+0x59650
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_59650(D2cExitState* s, const D2cExitEnv* e) {
    sendO(e, 2);
    if (s->state != 7) {
        s->flag77f8 = 1;
        s->tick = e->tick();
        s->state = 7;
    }
}

// 1.13c D2Client+0x596A0: `limit` is the caller's ESI (unrecovered register argument).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_596a0(D2cExitState* s, uint32_t limit, const D2cExitEnv* e) {
    if (s->state != 4 && s->state != 5 && s->state != 6) {
        uint32_t v = e->getStat(s->player, 0xE, 0);
        if (v > limit) v = limit;  // retail: <= limit keeps v, else clamps
        s->val77ee = (uint16_t)(v >> 16);
        s->val77ec = (uint16_t)v;
        e->send8(8, (uint16_t)(v >> 16), (uint16_t)v);
        return;
    }
    sendO(e, 7);
    s->state = 3;
}

// 1.13c D2Client+0x59740
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_59740(D2cExitState* s, int32_t param_1, const D2cExitEnv* e) {
    s->c2d0 = 0;
    if (s->boxA) { e->closebox(); s->boxA = nullptr; }
    if (s->boxB) { e->closebox(); s->boxB = nullptr; }
    if (s->state == 7) return;
    if (s->inGame) {
        e->uiMsg(0x17, 1, 0);
        s->be74 = 0;
        e->fn8bb90();
        if (s->player && param_1 != 0) {
            // *(player+0x10) != 0x11: unit mode read through the seam-free pointer
            if (*(const int32_t*)(uintptr_t)(s->player + 0x10) != 0x11) e->uiMsg(1, 2, 0);
        }
        if (s->aad94) e->uiMsg(5, 1, 0);
    }
    if (s->state != 0) {
        sendO(e, 2);
        s->state = 0;
    }
}

// 1.13c D2Client+0x59830
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_59830(D2cExitState* s, const D2cExitEnv* e) {
    if (s->state != 7) return 0;
    if (s->tick != 0xFFFFFFFFu) {
        if (s->tick + 30000u < e->tick()) {
            e->fn8ca20();
            s->state = 0;
            d2_client_59740(s, 0, e);
            e->fn14660();
            s->a8c9c = 0;
            s->c404 = 1;
            s->c3a4 = 1;
        }
    }
    return 1;
}

static void stamp(const D2cExitEnv* e, uint32_t** box) {
    e->fn88a70(0, 0, 0, 0);
    e->closebox();
    *box = nullptr;
}

// 1.13c D2Client+0x598A0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_598a0(D2cExitState* s, const D2cExitEnv* e) {
    stamp(e, &s->boxA);
    s->state = 7;
    s->tick = e->tick();
    sendO(e, 2);
    return 1;
}

// 1.13c D2Client+0x59910
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_59910(D2cExitState* s, const D2cExitEnv* e) {
    stamp(e, &s->boxB);
    s->state = 7;
    sendO(e, 2);
    s->tick = e->tick();
    return 1;
}

// 1.13c D2Client+0x59980
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_59980(D2cExitState* s, const D2cExitEnv* e) {
    stamp(e, &s->boxB);
    s->state = 3;
    sendO(e, 3);
    return 1;
}

// Shared tail of 599E0/59AC0: the box has been created; button list is added, then box validated.
static uint32_t* mkbox(uint32_t** slot, uint32_t cb, const uint32_t* btns, int n, const D2cExitEnv* e) {
    *slot = e->newBox(0x140, 100, cb, cb, 0, 1, 1, 0, 0);
    uint32_t* b = *slot;
    if (!b) e->fatal(0x529);
    b[0x17] = 1;
    e->addButton(b, e->getStr(), 0xF, 0, 1, 0, 0);
    for (int i = 0; i < n; i++) e->addButton(b, e->getStr(), 0xF, 0, 1, btns[i], 1);
    if (b[0x14] >= 0x80000000u) e->fatal(0x4F8);
    b[0x11] = 0xFFFFFFFFu;
    return b;
}

// 1.13c D2Client+0x599E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_599e0(D2cExitState* s, const D2cExitEnv* e) {
    if (s->boxA) e->fatal(0x13D);
    static const uint32_t btns[] = {0x598A0};
    mkbox(&s->boxA, 0x598A0, btns, 1, e);
}

// 1.13c D2Client+0x59AC0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_59ac0(D2cExitState* s, const D2cExitEnv* e) {
    if (s->boxB) return 0;
    static const uint32_t btns[] = {0x59980, 0x59910};
    mkbox(&s->boxB, 0x59910, btns, 2, e);
    return 1;
}

// 1.13c D2Client+0x59BC0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_59bc0(D2cExitState* s, const D2cExitEnv* e) {
    if (s->state > 2 && s->aad60 == 0) {
        int32_t ox = (int32_t)s->b9a0, oy = (int32_t)s->b9a4;
        int32_t a = (int32_t)s->mouseX, b = (int32_t)s->mouseY;
        if (ox + 0x111 <= a && a <= ox + 0x133 && 0xA8 - oy <= b && b <= 0xCA - oy) e->click(0xA7 - oy);
        if (ox + 0xDD <= a && a <= ox + 0xFF && 0x17F - oy <= b && b <= 0x1A1 - oy) e->click(0x17E - oy);
        if (ox + 0x111 <= a && a <= ox + 0x133 && 0x17F - oy <= b && b <= 0x1A1 - oy) e->click(0x17E - oy);
    }
    if (s->player) {
        uint32_t u = e->ord10331(s->player);
        bool lab = *(const int32_t*)(uintptr_t)(s->player + 0x10) == 0x11;
        if (!lab && u != 0) lab = e->ord10416(e->ord10826(u)) == 0;
        if (lab) {
            if (!s->inGame) return;
            sendO(e, 2);
            uint32_t was = s->inGame;
            e->ord10227();
            if (s->d5d0 != 0x26) e->fatal(0x139B);
            s->inGame = 0;
            if (was && d2_client_59830(s, e) == 0) {
                s->c418 = 0;
                s->c414 = 0;
                e->ord10006();
                e->fn3a4d0(s->a9e14, 0, 0);
                e->fn6f720();
                e->log1(e->ord10048(), 0x8BD18);
                e->fn6e2e0();
                d2_client_59740(s, 0, e);
                s->c434 = 1;
                if (s->d349c == 0) {
                    s->d349c = 1;
                    if (s->d3498) { e->log1(e->ord10048(), 0xF51EC); s->d3498 = 0; }
                    e->log2(e->ord10048(), 0xF51BC);
                }
            }
            s->be74 = 0;
            e->fn8bb90();
            s->state = 0;
            return;
        }
    }
    if ((s->state < 3 || s->state > 6) && s->state != 0) {
        uint32_t* b = s->state == 1 ? s->boxA : (s->state == 2 ? s->boxB : nullptr);
        if (b) e->boxUpdate(b);
    }
}
