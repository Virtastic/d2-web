// src/core/d2gfx/d2gfx_n124.cpp -- 1.13c D2Gfx.dll sub_88C0, sub_B580, sub_B730 from the 1.13c disassembly
// (alpha lane). Globals are named at their retail RVAs (image base 0x6FA80000). Win32, CRT and the sibling
// D2Gfx routines are host seams; the back-end interface table is read through fn pointers (slot = byte offset).
#include <cstdint>
#include <emscripten.h>

struct D2GfxTable {                       // renderer interface table; fastcall slots
    int32_t (*init)(uint32_t hInst);                            // +0x00
    void    (*getInfo)(void* a, void* b);                       // +0x04 (ecx=&be0, edx=&bf8)
    void    (*setMode)(uint32_t ecx, uint32_t edx);             // +0x30 (ecx=8, edx=arg4==1)
    void    (*fatal)(uint32_t a, uint32_t b, uint32_t c, uint32_t d); // +0x68 (called with first arg 0)
    void    (*setPalette)(void* pal);                           // +0x70 (ecx=&g78)
};
static D2GfxTable* g_table;               // +0x11268 DAT_6fa91268
static uint32_t g_hinst, g_wndproc, g_mode, g_arg4, g_pal_arg;  // +0x11250 +0x11254 +0x11258 +0x1125C +0x11260
static uint32_t g_hasPalette;             // +0x1D66C DAT_6fa9d66c
static uint32_t g_be0, g_be4;             // +0x10BE0 +0x10BE4
static uint8_t  g_pal78[1];               // +0x11278 palette block

struct D2GfxN124Env {
    void     (*error)(uint32_t id, const char* fmt);           // 0x7836(0x783C(id)) report
    void     (*exit_)(int32_t code);                           // 0x150B
    void     (*sub_84c0)();
    int32_t  (*sub_8360)(uint32_t res, uint32_t force);
    void     (*fatal7854)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    uint32_t (*loadIcon)(uint32_t hInst, uint32_t id);         // LoadImageA(hInst,id,IMAGE_ICON,0,0,0)
    uint32_t (*loadCursor)(uint32_t id);                       // LoadCursorA(0,0x7F00)
    uint32_t (*stockBrush)(uint32_t id);                       // GetStockObject(5)
    uint32_t (*registerClass)(const uint32_t* wc);             // RegisterClassA; 0 = failure
    uint32_t (*getLastError)();
    void     (*sub_7aa0)(uint32_t err);
    uint32_t (*has_ord10227)();                                // 0x7848 (cursor/icon variant)
    void     (*sub_9dc0)();
    void     (*atexit_b360)();
    int32_t  (*sub_b400)(uint32_t mode);
    const char* str_unsupported;                               // 0x6FA8E498 / 0x6FA90D18 pair, passed through
    const char* str_title;
};

// 1.13c D2Gfx+0x088C0 (116 B): ordinal 10005, `void __stdcall(uint32_t res)`. No table -> report 0x2BB and exit(-1).
// sub_84C0(); with a palette block present the arg is stored and slot +0x70 gets &g78; otherwise sub_8360(res,1)
// must succeed (else report string 0x6FA8E544 and exit(-1)) before slot +0x70.
// 1.13c D2Gfx+0x088C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_sub_88c0(uint32_t res, const D2GfxN124Env* e) {
    if (!g_table) { e->error(0x2bb, nullptr); e->exit_(-1); }
    e->sub_84c0();
    if (g_hasPalette) g_pal_arg = res;
    else if (!e->sub_8360(res, 1)) { e->error(0, e->str_unsupported); e->exit_(-1); }
    g_table->setPalette(g_pal78);
}

// 1.13c D2Gfx+0x0B580 (352 B): ordinal 10015, `int __stdcall(hInst, wndproc, mode, windowedFlag)`. Stores the four
// args, builds a WNDCLASS (style 0x20 only for mode 5, icon 0x66 or 0x67 by sub_7848, arrow cursor, NULL brush,
// class "Diablo II"), RegisterClass (fail: sub_7AA0(GetLastError), 0), sub_9DC0, atexit(0xB360), loads the back end
// (sub_B400, fail: 0), sets slot +0x30(8, flag==1), slot +0x00(hInst) (0: sub_7854(9,..), 0), clears +0x10BE4 for
// mode < 4, slot +0x04(&be0,&bf8).
// 1.13c D2Gfx+0x0B580
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_sub_b580(uint32_t hInst, uint32_t wndproc, uint32_t mode, uint32_t arg4,
                                                      const D2GfxN124Env* e) {
    g_hinst = hInst; g_wndproc = wndproc;
    uint32_t wc[10] = { mode == 5 ? 0x20u : 0u, wndproc, 0, 0, hInst,
                        e->loadIcon(g_hinst, e->has_ord10227() ? 0x67u : 0x66u), e->loadCursor(0x7f00),
                        e->stockBrush(5), 0, 0x6fa90c38u };
    if (!e->registerClass(wc)) { e->sub_7aa0(e->getLastError()); return 0; }
    e->sub_9dc0();
    e->atexit_b360();
    g_mode = mode; g_arg4 = arg4;
    if (!e->sub_b400(mode)) return 0;
    g_table->setMode(8, arg4 == 1);
    if (!g_table->init(hInst)) { e->fatal7854(9, 0, 0, 0, 0); return 0; }
    if ((int32_t)g_mode < 4) g_be4 = 0;
    g_table->getInfo(&g_be0, nullptr);
    return 1;
}

// 1.13c D2Gfx+0x0B730 (108 B): ordinal 10002, error/fallback. Modes 4..7 with a non-null +0x10BE0 forward
// (a, b, c) to slot +0x68 with a leading 0; modes 4..7 with null +0x10BE0 report string 0x99, other modes 0x98,
// then exit(-1).
// 1.13c D2Gfx+0x0B730
extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_sub_b730(uint32_t a, uint32_t b, uint32_t c, const D2GfxN124Env* e) {
    bool be = g_mode == 4 || g_mode == 5 || g_mode == 6 || g_mode == 7;
    if (!be || !g_be0) { e->error(be ? 0x99 : 0x98, nullptr); e->exit_(-1); }
    g_table->fatal(0, a, b, c);
}

// FN-probe seam for sub_88C0's non-fatal paths (table present): same body as d2gfx_sub_88c0 with explicit state.
// 1.13c D2Gfx+0x088C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_sub_88c0_fn(uint32_t res, uint32_t state, uint32_t* deferred,
        int32_t (*s8360)(uint32_t, uint32_t), void (*s84c0)(), void (*slot70)(uint32_t, uint32_t)) {
    s84c0();
    if (state) *deferred = res;
    else s8360(res, 1);
    slot70(0x6FA91278u, 0);
}
