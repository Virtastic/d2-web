// 1.13c D2Client+0x78820 (638 B): stat/count label draw beside a panel slot, from the cached decompile.
// Decompiler stack aliasing makes the Ordinal_10708 out-record ambiguous; modeled as a record
// {item=0x24? see spec}. Imports and globals are host seams (Env).
#include <cstdint>
#include <emscripten.h>

struct D2Client78820Rec {      // filled by Ordinal_10708 (ebp-relative locals in the decompile)
    int32_t sel;               // iStack_8
    int32_t mode;              // iStack_4 (forced to 3 when 10306==0 && 11146!=0 && sel!=-1)
    int32_t x;                 // iStack_c
    int32_t kind;              // iStack_20 (-1 selects FUN_6fae3550 else FUN_6fae3b50)
    int32_t count;             // iStack_24
    void* obj;                 // pvStack_2c
};

struct D2Client78820Env {
    uint32_t mouseY;                                         // DAT 0x6FBCBBFC
    int32_t (*gate)();                                       // FUN 0x6FAB17F0
    int32_t (*getY)();                                       // 0x77CA0 (iVar1)
    int32_t (*getX)();                                       // 0x77C50 (iVar2)
    void (*fill)(D2Client78820Rec*);                         // Ordinal_10708
    int32_t (*ord10306)(uint32_t);
    int32_t (*ord11146a)(D2Client78820Rec*);                 // Ordinal_11146()
    int32_t (*ord11146b)(uint32_t, void*);                   // Ordinal_11146(y,obj)
    void (*ord10019)(uint32_t* desc, int32_t x, int32_t y, int32_t a, int32_t b);
    void (*fn3550)(uint32_t, void*, int32_t);                // 0x33550
    void (*fn3b50)(void*, uint32_t, int32_t);                // 0x33B50
    void (*toUni)(uint16_t* dst, const char* src, int32_t n);
    void (*ord10184)();                                      // both branches identical call
    void (*ord10150)(int32_t x, uint32_t color, int32_t z);
    void (*fmtInt)(char* dst, int32_t v);                    // wsprintfA("%d")
};

// 1.13c D2Client+0x78820
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_78820(D2Client78820Env* e) {
    if (!e->gate()) return;
    int32_t y = e->getY(), x = e->getX();
    uint32_t my = e->mouseY;
    D2Client78820Rec r = {};
    r.sel = -1;
    e->fill(&r);
    int32_t m = r.mode;
    if (e->ord10306(my) == 0 && e->ord11146a(&r) != 0 && r.sel != -1) m = 3;
    uint32_t d[0x13] = {};
    d[0] = 0xffffffffu;
    e->ord10019(d, x, y, -1, 5);
    if (x < r.x && r.x < x + 0x30 && y - 0x30 < (int32_t)my && (int32_t)my < y) {
        if (r.kind == -1) e->fn3550(e->mouseY, r.obj, r.count);
        else e->fn3b50(r.obj, e->mouseY, r.count);
    }
    if (m != 0 || r.count != 0) {
        char s[40]; uint16_t u[20];
        e->fmtInt(s, r.count);
        e->toUni(u, s, 9);
        e->ord10184();
        int32_t c = e->ord11146b(e->mouseY, r.obj);
        e->ord10150(y + 0xc, c > 0 ? 3 : (c < 0 ? 1 : 0), 0);
    }
}
