// src/core/d2game/d2game_batch_dc780.cpp -- 1.13c D2Game.dll 0xDC780 and 0xDCC20: the duration-state appliers behind the
// damage-over-time effects, transcribed from the retail disassembly (base 0x6FC20000). Each takes the damage record in EAX, the
// duration on the stack (ret 4) and every callee through a seam, so the replay can put retail's answers behind them.
// Offsets are the 32-bit retail layout.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "../fn/hookenv.h"

struct D2GameDC780Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*getTimer)(uint32_t unit, uint32_t tag);                              // 0xA5D4
    int32_t  (*resist)(uint32_t unit, uint32_t game);                               // 0xDAE20 (EAX = unit, stack game)
    uint32_t (*diffRow)(uint32_t diffByte);                                         // 0xA7BA
    uint32_t (*setState)(uint32_t unit, uint32_t state, uint32_t on);               // 0xA3CA
    uint32_t (*newTimer)(uint32_t g1c, uint32_t kind, uint32_t end, uint32_t a, uint32_t b); // 0xA6A0
    uint32_t (*schedule)(uint32_t game, uint32_t unit, uint32_t ev, uint32_t end);  // 0x8EA80 (ECX = game, EDX = unit, stack ev end 0 0 0)
    void     (*tagTimer)(uint32_t t, uint32_t tag);                                 // 0xA5B6
    void     (*setCallback)(uint32_t t, uint32_t cb);                               // 0xA75A
    void     (*attach)(uint32_t unit, uint32_t t, uint32_t on);                     // 0xA60A
    void     (*setStat)(uint32_t t, uint32_t stat, uint32_t v, uint32_t layer);     // 0xA76C
    void     (*finish)(uint32_t unit, uint32_t str, uint32_t line);                 // 0xA59E
    int32_t  (*timerEnd)(uint32_t t);                                               // 0xA790
    void     (*setEnd)(uint32_t t, uint32_t end);                                   // 0xA778
    uint32_t (*classHook)(uint32_t cls, uint32_t v);                                // 0x213B0 (EAX = class, EDX = v)
    uint32_t (*skillFlag)(uint32_t unit, uint32_t v);                               // 0xA580
    uint32_t (*classRow)(uint32_t cls);                                             // 0x1220 (EAX = class)
    uint32_t (*unitFlag)(uint32_t unit);                                            // 0xA7F6
    uint32_t cbDB4C0, strDB1B0EC, cbBF7A0;                                          // callback / string module addresses as recorded
};

static inline uint32_t dcR(uint32_t a, uint32_t off) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + off), 4); return v; }
static inline void dcW(uint32_t a, uint32_t off, uint32_t v) { memcpy((void*)(uintptr_t)(a + off), &v, 4); }

// the record's LCG (seed lo at +0x20, hi at +0x24): returns the roll 0..99
static uint32_t dcRoll(uint32_t base) {
    uint64_t p = (uint64_t)dcR(base, 0x20) * 0x6ac690c5ull + dcR(base, 0x24);
    dcW(base, 0x24, (uint32_t)(p >> 32));
    dcW(base, 0x20, (uint32_t)p);
    return (uint32_t)p % 100;
}

void d2game_DC780_113c(uint32_t rec, uint32_t unit, int32_t dur, const D2GameDC780Env& e) {
    if (dur <= 0) return;
    uint32_t timer = e.getTimer(unit, 0xb);
    if (!rec) { e.fatal(0xa2a); return; }
    uint32_t game = dcR(rec, 0x80);
    int32_t res = e.resist(unit, game);
    if (res == 0) return;
    if (unit && dcR(unit, 0) == 1 && res < 0) {
        uint32_t row = e.diffRow(*(uint8_t*)(uintptr_t)(game + 0x6d));
        int32_t d = (int32_t)dcR(row, 0x18);
        if (d) dur = dur / d;
    }
    if (dur <= 1) dur = 1;
    uint32_t end = dcR(game, 0xa8) + (uint32_t)dur;
    if (timer == 0) {
        e.setState(unit, 0xb, 1);
        uint32_t t = e.newTimer(dcR(game, 0x1c), 2, end, dcR(rec, 0), dcR(rec, 0xc));
        e.schedule(game, unit, 0xc, end);
        e.tagTimer(t, 0xb);
        e.setCallback(t, e.cbDB4C0);
        e.attach(unit, t, 1);
        e.setStat(t, 0x43, (uint32_t)res, 0);
        e.setStat(t, 0x44, (uint32_t)res, 0);
        e.setStat(t, 0x45, (uint32_t)res, 0);
        e.finish(unit, e.strDB1B0EC, 0x1d8);
    } else if (e.timerEnd(timer) < (int32_t)end) {
        e.setEnd(timer, end);
        e.schedule(game, unit, 0xc, end);
    }
    uint32_t roll = dcRoll(unit);
    if (roll < 20 && unit && dcR(unit, 0) == 1 && e.classHook(dcR(unit, 4), 0x13) == 0) {
        e.setState(unit, 0x6b, 1);
        return;
    }
    e.setState(unit, 0x6b, 0);
}

// EAX on exit: the record on a non-positive duration, the roll's quotient (seed / 100) on the 90% skip, the 0xA580 or
// 0x1220 result on their early outs, else the last callee's result (0xA3CA on a new timer, 0x8EA80 on an extended one).
uint32_t d2game_DCC20_113c(uint32_t rec, uint32_t unit, int32_t dur, const D2GameDC780Env& e) {
    if (dur <= 0) return rec;
    bool clampPlayer = false;
    if (unit && dcR(unit, 0) == 1) {
        uint32_t pd = dcR(unit, 0x14);
        if (pd && (*(uint8_t*)(uintptr_t)(pd + 0x16) & 8)) {
            if (dcRoll(rec) < 0x5a) return dcR(rec, 0x20) / 100;
        }
        if (const uint32_t f = e.skillFlag(unit, 0)) return f;
        uint32_t row = e.classRow(dcR(unit, 4));
        uint16_t w; memcpy(&w, (const void*)(uintptr_t)(row + 0x32), 2);
        if (w == 0) return row;
        if (e.unitFlag(unit) != 0 && dur >= 0xd) { dur = 0xd; clampPlayer = true; }
    }
    if (!clampPlayer && dur > 0xfa) dur = 0xfa;
    uint32_t timer = e.getTimer(unit, 0x15);
    if (!rec) { e.fatal(0xa2a); return 0; }
    uint32_t game = dcR(rec, 0x80);
    uint32_t end = dcR(game, 0xa8) + (uint32_t)dur;
    if (timer == 0) {
        uint32_t t = e.newTimer(dcR(game, 0x1c), 2, end, dcR(rec, 0), dcR(rec, 0xc));
        e.schedule(game, unit, 0xc, end);
        e.tagTimer(t, 0x15);
        e.setCallback(t, e.cbBF7A0);
        e.attach(unit, t, 1);
        return e.setState(unit, 0x15, 1);
    }
    e.setEnd(timer, end);
    return e.schedule(game, unit, 0xc, end);
}

// replay entries: r[0]=rec (EAX), r[1]=unit (ESI for 0xDC780, EDI for 0xDCC20), r[2]=duration, r[3]=log, r[4]=sc
static HookEnv* g_dcz;
static uint32_t z_timer(uint32_t u, uint32_t t) { return g_dcz->h(0xA5D4, 0, 0, 0, 0, 0, u, t); }
static int32_t  z_res(uint32_t u, uint32_t g) { return (int32_t)g_dcz->h(0xDAE20, u, 0, 0, 0, 0, g); }
static uint32_t z_row(uint32_t d) { return g_dcz->h(0xA7BA, 0, 0, 0, 0, 0, d); }
static uint32_t z_state(uint32_t u, uint32_t s, uint32_t o) { return g_dcz->h(0xA3CA, 0, 0, 0, 0, 0, u, s, o); }
static uint32_t z_new(uint32_t a, uint32_t k, uint32_t e, uint32_t b, uint32_t c) { return g_dcz->h(0xA6A0, 0, 0, 0, 0, 0, a, k, e, b, c); }
static uint32_t z_sched(uint32_t g, uint32_t u, uint32_t ev, uint32_t e) { return g_dcz->h(0x8EA80, 0, g, u, 0, 0, ev, e, 0, 0, 0); }
static void     z_tag(uint32_t t, uint32_t g) { g_dcz->h(0xA5B6, 0, 0, 0, 0, 0, t, g); }
static void     z_cb(uint32_t t, uint32_t c) { g_dcz->h(0xA75A, 0, 0, 0, 0, 0, t, c); }
static void     z_att(uint32_t u, uint32_t t, uint32_t o) { g_dcz->h(0xA60A, 0, 0, 0, 0, 0, u, t, o); }
static void     z_stat(uint32_t t, uint32_t s, uint32_t v, uint32_t l) { g_dcz->h(0xA76C, 0, 0, 0, 0, 0, t, s, v, l); }
static void     z_fin(uint32_t u, uint32_t s, uint32_t l) { g_dcz->h(0xA59E, 0, 0, 0, 0, 0, u, s, l); }
static int32_t  z_tend(uint32_t t) { return (int32_t)g_dcz->h(0xA790, 0, 0, 0, 0, 0, t); }
static void     z_setend(uint32_t t, uint32_t e) { g_dcz->h(0xA778, 0, 0, 0, 0, 0, t, e); }
static uint32_t z_chook(uint32_t c, uint32_t v) { return g_dcz->h(0x213B0, c, 0, v); }
static uint32_t z_skill(uint32_t u, uint32_t v) { return g_dcz->h(0xA580, 0, 0, 0, 0, 0, v, u); }
static uint32_t z_crow(uint32_t c) { return g_dcz->h(0x1220, c); }   // retail calls 0x6FC21220 = rva 0x1220
static uint32_t z_uflag(uint32_t u) { return g_dcz->h(0xA7F6, 0, 0, 0, 0, 0, u); }
static void     z_fatal(uint32_t) { abort(); }
static D2GameDC780Env zenv() {
    D2GameDC780Env e{};
    e.fatal = z_fatal; e.getTimer = z_timer; e.resist = z_res; e.diffRow = z_row; e.setState = z_state; e.newTimer = z_new;
    e.schedule = z_sched; e.tagTimer = z_tag; e.setCallback = z_cb; e.attach = z_att; e.setStat = z_stat; e.finish = z_fin;
    e.timerEnd = z_tend; e.setEnd = z_setend; e.classHook = z_chook; e.skillFlag = z_skill; e.classRow = z_crow; e.unitFlag = z_uflag;
    e.cbDB4C0 = 0xB2000000u | 0xDB4C0; e.strDB1B0EC = 0xB2000000u | 0xF90EC; e.cbBF7A0 = 0xB2000000u | 0x9F7A0;
    return e;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_dc780(uint32_t* r) {
    HookEnv he((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[5]); g_dcz = &he;
    D2GameDC780Env e = zenv();
    d2game_DC780_113c(r[0], r[1], (int32_t)r[2], e);
    r[4] = 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_dcc20(uint32_t* r) {
    HookEnv he((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[5]); g_dcz = &he;
    D2GameDC780Env e = zenv();
    r[4] = d2game_DCC20_113c(r[0], r[1], (int32_t)r[2], e);
}
