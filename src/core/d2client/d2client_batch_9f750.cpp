// src/core/d2client/d2client_batch_9f750.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows reimplemented from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, batch 9f750). Base 0x6FAB0000. Retail globals are
// passed in (native pointers, never 32-bit casts); imports are host seams, a null seam keeps retail's flow.
// Register-argument functions keep retail's register order. No spec, no vector yet.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

// Button slot table, stride 0x28, first slot at RVA(0x6fb8ec72) = 0x...8EC72 from here ("slot" base):
//   +0x00 left, +0x04 right, +0x08 top, +0x0C bottom (int32), +0x14 down flag (ec86), +0x15 hot flag (ec87).
// Count is DAT_6fb8ecb0 (int32).
struct D2C9f750Slots { uint8_t* base; int32_t count; };
static int32_t sl_i32(const D2C9f750Slots& s, int i, int off) { int32_t v; std::memcpy(&v, s.base + i * 0x28 + off, 4); return v; }
static void sl_clear(const D2C9f750Slots& s) { for (int i = 0; i < s.count; ++i) { s.base[i * 0x28 + 0x14] = 0; s.base[i * 0x28 + 0x15] = 0; } }

struct D2C9f750Env {
    void*   (*alloc_6f6c0)(void);                       // 0xBF6C0
    void    (*fn_88a70)(int, int, int, int);           // 0x88A70
    void*   (*make_dialog)(int, int, void*, int, int, int, int, int, int); // 0x53E40
    void    (*dialog_init)(void*, void*, int, int, int, int, int);        // 0x536F0
    void    (*fatal)(int msgId);                       // Ordinal_10265 + 10024 + _exit(-1)
    void*   (*get_text)(void);                         // Ordinal_10003
    void    (*send_packet)(const uint8_t* p13);        // 0x143E0
    void    (*fn_f5fb0)(void);                         // 0x2F5FB0
    void    (*free_dialog)(void);                      // 0x53D20
    void    (*cleanup_f950)(void);                     // 0x9F950
    void    (*free_char)(void*);                       // 0x6FAECA00
    void    (*reset_72790)(int, int, int);             // 0xC2790
    void    (*free_handle)(void*);                     // Ordinal_10065
    void    (*free_10043)(int, int);                   // Ordinal_10043
    int     (*ord_10048)(void);
    void    (*ord_11017)(void*);
    void    (*dialog_cb_9fd70)(void);                  // 0x9FD70
};

// 1.13c D2Client+0x9F750
// g[0]=bb214 state, g[1]=bb21c, g[2]=bb218, g[3]=bb228; handles = bcbc0c/bcbc10.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f750(int32_t* g, void** h0, void** h1, uint8_t* slotBase, int32_t slotCount, const D2C9f750Env* env) {
    g[0] = 0; g[1] = 0; g[2] = 0; g[3] = 0;
    *h0 = env->alloc_6f6c0();
    *h1 = env->alloc_6f6c0();
    sl_clear({slotBase, slotCount});
}

// 1.13c D2Client+0x9F7B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f7b0(int32_t x, int32_t y, uint8_t* slotBase, int32_t slotCount, const D2C9f750Env* env) {
    D2C9f750Slots s{slotBase, slotCount};
    sl_clear(s);
    for (int i = 0; i < slotCount; ++i) {
        if (sl_i32(s, i, 0) <= x && x <= sl_i32(s, i, 4) && sl_i32(s, i, 8) <= y && y <= sl_i32(s, i, 0xC)) {
            env->fn_88a70(0, 0, 0, 0);
            s.base[i * 0x28 + 0x15] = 1;
            s.base[i * 0x28 + 0x14] = 1;
        }
    }
}

// 1.13c D2Client+0x9F830
// st = bb214 state, unitSel = bbfc? (DAT_6fbcbbfc, unit; id at +0xC), dlgOut = bb224, flag210 = bb210,
// v21c/v220 = bb21c/bb220. Returns 1 when it opened the dialog.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_9f830(int32_t* st, const uint8_t* unitSel, void** dlgOut, int32_t flag210,
                                                         int32_t v21c, int32_t v220, const D2C9f750Env* env) {
    if (*st != 2) return 0;
    env->fn_88a70(0, 0, 0, 0);
    *dlgOut = env->make_dialog(0x96, 0x82, nullptr, 0, 1, 0, 1, 0, 0);
    if (!*dlgOut) { env->fatal(0x529); return 0; }
    reinterpret_cast<uint32_t*>(*dlgOut)[0x17] = 0;
    env->dialog_init(*dlgOut, env->get_text(), 0xf, 0, 1, 0, 0);
    *st = 3;
    if (flag210 == 0) {
        int32_t id = -1; if (unitSel) std::memcpy(&id, unitSel + 0xC, 4);
        
        // retail's packet is 17 bytes: 'D', id, v21c, v220, 3
        uint8_t q[17]; q[0] = 'D'; std::memcpy(q + 1, &id, 4); std::memcpy(q + 5, &v21c, 4); std::memcpy(q + 9, &v220, 4);
        int32_t three = 3; std::memcpy(q + 13, &three, 4);
        env->send_packet(q);
        return 1;
    }
    env->fn_f5fb0();
    return 1;
}

// 1.13c D2Client+0x9F9E0
// unitSel = DAT_6fbcbbfc (unit; +0x60 is the arg of Ordinal_11017). tab = 8 entries x 0xC bytes at ecbc, the
// first dword of each entry a char to free.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9f9e0(int32_t* st, void* unitSel60, void** dlg, void** chars, int32_t* bcc2fc, const D2C9f750Env* env) {
    *st = 0;
    if (unitSel60) env->ord_11017(unitSel60);
    env->cleanup_f950();
    if (env->ord_10048() == 0) { env->fatal(0x18c); return; }
    for (uint32_t o = 0; o < 0x60; o += 0xC) env->free_char(chars[o / 0xC]);
    if (*dlg) { env->free_dialog(); *dlg = nullptr; }
    env->reset_72790(0xe, 1, 0);
    *bcc2fc = 0;
}

// 1.13c D2Client+0x9FA90
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9fa90(int32_t* st, void** h0, void** h1, void (*close9f9e0)(void), const D2C9f750Env* env) {
    if (*st != 0) close9f9e0();
    void** hs[2] = {h0, h1};
    for (void** h : hs) if (*h) { env->free_handle(*h); env->free_10043(0x7a, 0); *h = nullptr; }
}

// 1.13c D2Client+0x9FE50
// EDI = y (unaff), param_1 = x. Slot 0 -> 9F830, others -> 9FD70.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_9fe50(int32_t x, int32_t y, uint8_t* slotBase, int32_t slotCount,
                                                         uint32_t (*open9f830)(void), const D2C9f750Env* env) {
    D2C9f750Slots s{slotBase, slotCount};
    uint32_t r = 0;
    for (int i = 0; i < s.count; ++i) {
        if (s.base[i * 0x28 + 0x15] && sl_i32(s, i, 0) <= x && x <= sl_i32(s, i, 4) && sl_i32(s, i, 8) <= y && y <= sl_i32(s, i, 0xC)) {
            env->fn_88a70(0, 0, 0, 0);
            if (i == 0) open9f830(); else env->dialog_cb_9fd70();
            r = 1;
        }
    }
    sl_clear(s);
    return r;
}

// Fog/D2Common imports for the rows below (ordinals as in the decompile).
struct D2C9f750Env2 {
    void* (*item_slot)(void* unit, int slot);       // Ordinal_11139
    int   (*ord_11006)(void*);
    int   (*ord_11088)(void*);
    int   (*ord_10744)(void*, int);
    void* (*ord_10708)(int);
    int   (*ord_10631)(void*);
    void  (*ord_11031)(void*, int);
    void  (*ord_11103)(void);
    void  (*size_10561)(void* p, int32_t* wh);      // Ordinal_10561: wh[0]=w, wh[1]=h
    void* (*ord_11013)(int, int, int, int, int);
    void  (*ord_10807)(void);
    void  (*ord_10188)(void*, int, int, int);
    void  (*ord_10819)(void);
    int   (*ord_10276)(uint32_t, void*);
    void* (*ord_10494)(uint32_t unit, int skill);
    void* (*ord_10871)(uint32_t unit, int skill);
    int   (*ord_10680)(void*, int, int);
};

// 1.13c D2Client+0xA0660
// EBX = unit; *state is the quest/state word. Retail only acts while *state == 0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a0660(int32_t* state, void* unit, const D2C9f750Env2* e) {
    if (*state != 0) return;
    auto pick = [&](int slot) -> void* {
        void* it = e->item_slot(unit, slot);
        if (it && (e->ord_11006(it) == 0 || e->ord_11088(it) == 0x26)) it = nullptr;
        return it;
    };
    void* a = pick(4);
    void* b = pick(5);
    if (e->ord_10744(a, 0x2d) && e->ord_10744(b, 0x2d)) {
        void* u = e->ord_10708(-1);
        if (e->ord_10631(u) == 0) { *state = 5; e->ord_11031(u, 5); e->ord_10708(-1); e->ord_11103(); return; }
        e->ord_11031(u, 0);
        e->ord_11103();
    }
}

// 1.13c D2Client+0xA0750
// Register inputs in retail order: ECX=param_1, EDX=param_2, EAX=in_EAX, EBX, ESI, EDI.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a0750(void* p1, int32_t p2, int32_t eax, int32_t ebx, int32_t esi, int32_t edi, const D2C9f750Env2* e) {
    int32_t wh[2] = {0, 0};
    e->size_10561(p1, wh);
    if (p2 + edi < esi && wh[0] + esi < p2 - edi) return 0;
    if (ebx <= eax + edi) return 1;
    return (eax - edi <= wh[1] + ebx) ? 1 : 0;
}

// 1.13c D2Client+0xA07C0
// EBX -> a, EDI -> b: (a,b) = (b, -a), then halved together until a^2+b^2 <= 3.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a07c0(int32_t* a, int32_t* b) {
    int32_t v3 = -*a, v2 = *b;
    while (v3 * v3 + v2 * v2 > 3) { v2 /= 2; v3 /= 2; }
    *a = v2; *b = v3;
}

// 1.13c D2Client+0xA0800
// ESI -> a, param_2 -> b: scale both by 4 if a^2+b^2 < 4, then by 2 if the (new) sum < 0x10.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a0800(int32_t* a, int32_t* b) {
    int32_t x = *a, s = *b * *b + x * x;
    if (s < 4) { *a = x * 4; *b = *b * 4; x = *a; s = *b * *b + x * x; }
    if (s < 0x10) { *a = x * 2; *b = *b << 1; }
}

// 1.13c D2Client+0xA0880
// EDI = unit or null. Ordinal_11013(0,4,0,unit?[0]:6,unit?[3]:-1).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a0880(const uint32_t* unit, const D2C9f750Env2* e) {
    int a = unit ? (int)unit[0] : 6, b = unit ? (int)unit[3] : -1;
    void* p = e->ord_11013(0, 4, 0, a, b);
    if (p) { e->ord_10807(); e->ord_10188(p, 0x44, 1, 0); e->ord_10819(); }
}

// 1.13c D2Client+0xA08E0
// tables = sgptDataTables (native); handlers = ea48 array of callbacks (count ec50); unit = stack word.
// Skill list: int16 array at *(tables+0x184), count int16 at tables+0x188; skills.txt rows stride 0x3c at
// *(tables+0xBC), count int32 at +0xC4, row +0x34 int16 = handler index.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a08e0(void* thisp, uint32_t param1, const uint8_t* tables, uint32_t unit,
                                                     void (* const* handlers)(uint32_t), int32_t handlerCount, const D2C9f750Env2* e) {
    if (!e->ord_10276(param1, thisp)) return;
    auto rd16 = [](const uint8_t* p) { int16_t v; std::memcpy(&v, p, 2); return (int)v; };
    auto rd32 = [](const uint8_t* p) { int32_t v; std::memcpy(&v, p, 4); return (int)v; };
    auto rdp = [](const uint8_t* p) { const uint8_t* v; std::memcpy(&v, p, sizeof v); return v; };
    for (int i = 0; i < rd16(tables + 0x188); ++i) {
        int sk = rd16(rdp(tables + 0x184) + i * 2);
        void* has = e->ord_10494(unit, sk);
        if (!has || sk < 0 || sk >= rd32(tables + 0xC4)) continue;
        const uint8_t* row = rdp(tables + 0xBC) + sk * 0x3c;
        int idx = rd16(row + 0x34);
        if (idx < 0 || idx >= handlerCount || !handlers[idx]) continue;
        uint32_t arg = 1;
        void* sd = e->ord_10871(unit, sk);
        if (sd) { e->ord_10680(sd, 0x15e, 0); arg = (uint32_t)e->ord_10680(sd, 0x15f, 0); }
        handlers[idx](arg);
    }
}
