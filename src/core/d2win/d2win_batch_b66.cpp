// src/core/d2win/d2win_batch_b66.cpp -- ten 1.13c D2Win rows from the cached Ghidra decompiles.
// Alpha batch lane: implementation + marker only. Skipped: 0x0BE00 (eight register-only calls to
// 0xB250, eax per call not recoverable), 0x07E60 (Sleep / direct Win32). Callees are env seams.
// Widgets are int32 arrays exactly as the decompile indexes them; widget pointers are 32-bit
// handles converted by env->p (handle -> pointer) / env->h (pointer -> handle).
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <emscripten.h>

struct D2WinB66Env {
    void (*fatal)(int32_t id);                         // Ordinal_10265(id) -> Ordinal_10024("",s) -> _exit(-1)
    void* (*alloc)(int32_t size, int32_t line);        // Ordinal_10042(size,0)
    void (*free_)(int32_t line);                       // Ordinal_10043(line,0)
    void* (*allocSrc)(int32_t size, const char* file, int32_t line);  // Ordinal_401
    void (*freeSrc)(void* p, const char* file, int32_t line);         // Ordinal_403
    int32_t* (*p)(int32_t handle);
    int32_t (*h)(const void* ptr);
    int32_t* listHead;                                 // DAT_6f9014a0 (widget list head handle)
    const uint32_t* bitMasks;                          // gdwBitMasks
    int32_t cb1520, cb1390;                            // 0x11 callbacks (draw, free)
    int32_t cb8d70, cb8c60, cb9840;                    // 0x9C00 callbacks / key-handler address
    int32_t (*textWidth)(const uint16_t* s, int32_t n);  // 0x12080 / 0x12080-family
    int32_t (*sub_9200)(int32_t* w);
    void (*sub_8B50)(void);
    int32_t* gCurFont;                                 // DAT_6f900c68
    int32_t (*sub_93C0)(int32_t* w, const uint16_t* text, int32_t flag, int32_t mode);
    int32_t* (*sub_B160)(int32_t a, int32_t b, int32_t c, int32_t d);
    void (*sub_2FE0)(int32_t font);                    // 0x12FE0
    const uint8_t* (*glyph)(void);                     // (*DAT_6f901508)()
    const char* (*fontName)(int32_t idx, int32_t ingame, int32_t* dirTag, const char** dirOut);
    int32_t (*sub_7C60)(int32_t, int32_t);
    void (*sub_2280)(int32_t);
    int32_t* fontTbl;                                  // &DAT_6f901514, stride 5 ints
};

static void u16cpy(uint16_t* d, const uint16_t* s) { while ((*d++ = *s++)) {} }
static int32_t u16len(const uint16_t* s) { int32_t n = 0; while (s[n]) n++; return n; }

// 1.13c D2Win+0x0F7E0 -- type-6 widget: set [0x18], [0x99], copy text at +0x19*4.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_b66_F7E0(int32_t* w, int32_t v18, const uint16_t* text, int32_t v99, const D2WinB66Env* e) {
    if (*w != 6) e->fatal(0x84);
    w[0x18] = v18; w[0x99] = v99;
    u16cpy((uint16_t*)(w + 0x19), text);
}

// 1.13c D2Win+0x0F5E0 -- type-6 widget: set [0x9c].
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_b66_F5E0(int32_t* w, int32_t v, const D2WinB66Env* e) {
    if (*w != 6) e->fatal(0x100);
    w[0x9c] = v;
}

// 1.13c D2Win+0x0B340 -- *p = v, null pointer is fatal.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_b66_B340(int32_t* p, int32_t v, const D2WinB66Env* e) {
    if (!p) e->fatal(0xd97);
    *p = v;
}

// 1.13c D2Win+0x085A0 -- type-4 widget whose [0x24] child is type 5: child[0x17] = v. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_b66_85A0(int32_t* w, int32_t v, const D2WinB66Env* e) {
    if (!w) e->fatal(0x5aa);
    if (*w != 4) e->fatal(0x5ab);
    int32_t* c = e->p(w[0x24]);
    if (*c != 5) e->fatal(0x8b);
    c[0x17] = v;
    return 1;
}

static void appendWidget(int32_t* w, const D2WinB66Env* e) {
    if (*e->listHead == 0) { *e->listHead = e->h(w); return; }
    int32_t* t = e->p(*e->listHead);
    while (t[0xf]) t = e->p(t[0xf]);
    t[0xf] = e->h(w);
}

// 1.13c D2Win+0x115A0 -- new type-9 widget (0x150 bytes), appended to the list.
// x=ECX, y=EDX, param_3 at stack; hidden: width=EBX, height=caller-pushed (unaff_retaddr).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t* d2win_b66_115A0(int32_t x, int32_t y, int32_t p3, int32_t width, int32_t height, const D2WinB66Env* e) {
    int32_t* w = (int32_t*)e->alloc(0x21, 0);
    for (int i = 0; i < 0x54; i++) w[i] = 0;
    w[0] = 9;
    w[2] = (w[2] | e->bitMasks[0]) | e->bitMasks[2];
    w[5] = width; w[0xf] = 0; w[10] = 0; w[0xb] = 0; w[0xc] = 0; w[0xd] = p3;
    w[3] = x; w[4] = y; w[6] = height;
    w[7] = e->cb1520; w[9] = e->cb1390;
    *(uint8_t*)(w + 0x10) = 0;
    w[0x50] = w[0x51] = 0;
    w[0x52] = (width - 0x1d4) / 2 + x;
    w[0x53] = (height - 0x3c) / 2 + y;
    appendWidget(w, e);
    return w;
}

// 1.13c D2Win+0x09C00 -- new type-4 widget (0xAC bytes). this=ECX=x(w0); hidden: ebx=w[3], ebp=w[4],
// retaddr=w[6] (stack). Decompile names: param_1..4 = [0x10],[0x11],[1],[0xd].
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t* d2win_b66_9C00(int32_t thisX, int32_t p1, int32_t p2, int32_t p3, int32_t p4, uint32_t flags,
                        const int32_t* p6, const int32_t* p7,
                        int32_t ebx, int32_t ebp, int32_t stk, const D2WinB66Env* e) {
    int32_t* w = (int32_t*)e->alloc(99, 0);
    for (int i = 0; i < 0x2b; i++) w[i] = 0;
    int32_t x = thisX;
    if ((flags & 4) && !p7) x -= 0x1a;
    if (flags & 1) flags |= 0x20;
    w[0] = 4;
    w[2] = (w[2] | e->bitMasks[0]) | e->bitMasks[2];
    w[1] = p3; w[5] = x; w[3] = ebx; w[0x10] = p1; w[4] = ebp; w[0xd] = p4; w[0x11] = p2;
    w[0x17] = (int32_t)flags; w[6] = stk;
    w[7] = e->cb8d70; w[9] = e->cb8c60;
    w[10] = 0; w[0xb] = 0;
    w[0xc] = (flags & 1) ? e->cb9840 : 0;
    w[0x18] = -1; w[0xf] = 0; w[0x14] = 0; w[0x12] = 0; w[0x13] = 0; w[0x15] = 0; w[0x16] = 0; w[0x19] = 1;
    if (!p6) { w[0x25] = 1; w[0x26] = 0; } else { w[0x25] = p6[0]; w[0x26] = p6[1]; }
    e->sub_8B50();
    w[0x2a] = *e->gCurFont;
    if (!(flags & 4)) { w[0x24] = 0; appendWidget(w, e); return w; }
    w[1] = 0;
    int32_t a, b, c;
    if (!p7) { b = ebp - 2; a = ebx + 8 + x; c = stk - 0x10; }
    else { a = p7[0]; b = p7[1]; c = p7[2]; }
    int32_t* sb = e->sub_B160(a, b, 10, c);
    w[0x24] = e->h(sb);
    if (*sb != 5) e->fatal(0x7f);
    sb[0x15] = e->h(w);
    int32_t n = e->sub_9200(w) - 1;
    if (n < 1) n = 1;
    int32_t* ch = e->p(w[0x24]);
    if (*ch != 5) e->fatal(0xca);
    ch[0x14] = n;
    appendWidget(w, e);
    return w;
}

// 1.13c D2Win+0x099F0 -- text-box: wrap text at width limit, add the two halves via 0x93C0.
// this=ECX widget, text=ESI (hidden), limit=EBP (hidden), param_1 = mode, param_2 = kind.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_b66_99F0(int32_t* w, const uint16_t* text, int32_t limit, int32_t mode, int32_t kind,
                       const D2WinB66Env* e) {
    static const char* F = "..\\Source\\D2Win\\Src\\D2WinTextBox.cpp";
    if (!w) e->fatal(0x482);
    if (*w != 4) e->fatal(0x483);
    uint16_t sp[6] = {0x20, 0x20, 0x20, 0x20, 0x20, 0};  // only measured (decompile discards result)
    if (kind == 2) e->textWidth(sp, u16len(sp));
    uint16_t* all = (uint16_t*)e->allocSrc(0x800, F, 0x497);
    u16cpy(all, text);
    int32_t len = u16len(all);
    const uint16_t* lastSp = nullptr;
    int32_t cut = 0, i = 0;
    if (len > 0) {
        const uint16_t* cur = all;
        do {
            if (*cur == 0x20) lastSp = cur;
            cut = i + 1;
            if (e->textWidth(text, cut) >= limit) {
                cut = i;
                if (lastSp) cut = (int32_t)(lastSp - all);
                break;
            }
            cur++; i = cut;
        } while (cut < len);
    }
    uint16_t* a = (uint16_t*)e->allocSrc(0x800, F, 0x4b7);
    uint16_t* b = (uint16_t*)e->allocSrc(0x800, F, 0x4b8);
    for (int32_t k = 0; k < cut; k++) a[k] = all[k];
    a[cut] = 0;
    // decompile copies from the register-held source for the tail; same buffer in the original
    for (int32_t k = 0; k < len - cut; k++) b[k] = all[cut + k];
    b[len - cut] = 0;
    e->freeSrc(all, F, 0x4bf);
    e->sub_93C0(w, a, 1, mode);
    if (mode == 1) mode = 2;
    e->sub_93C0(w, b, 0, mode);
    e->freeSrc(a, F, 0x4c4);
    e->freeSrc(b, F, 0x4c5);
    return 1;
}

// 1.13c D2Win+0x12350 -- build "DATA\LOCAL\<lang>\" + dirTag + fontName into dst. size=param_1,
// fontIdx=EBX, dst=EDI (hidden). lang/ingame/tag selection is in env->fontName.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_b66_12350(int32_t size, int32_t fontIdx, char* dst, const char* lang, int32_t ingame,
                     const char* tagDefault, const char* tagChat, const char* nameFor,
                     const char* nameChat, const D2WinB66Env* e) {
    char dir[260];
    snprintf(dir, sizeof dir, "%s\\%s\\", "DATA\\LOCAL", lang);
    int32_t n = 0; while (dir[n]) n++;
    if (size <= n + 0x13) e->fatal(0x7b);   // (end - (buf+1)) + 0x14 == strlen + 0x13
    const char* tag = tagDefault; const char* name = nameFor;
    if (ingame && fontIdx == 0xd) { tag = tagChat; name = nameChat; }
    snprintf(dst, size, "%s%s%s", dir, tag, name);
}

// 1.13c D2Win+0x12B40 -- slot idx (EAX hidden): reset and re-create its 3-int entry.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_b66_12B40(int32_t idx, const D2WinB66Env* e) {
    e->sub_2280(0x4f);
    int32_t* s = e->fontTbl + idx * 5;
    if (s[0]) e->free_(0xb3);
    int32_t v = e->sub_7C60(0, 0);
    s[0] = v; s[1] = v; s[2] = v + 0xc;
}

// 1.13c D2Win+0x13AC0 -- edit-box scroll window [0x4c,0x50] recompute. ESI = edit widget (hidden).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_b66_13AC0(uint8_t* esi, const D2WinB66Env* e) {
    auto I = [&](int off) -> int32_t& { return *(int32_t*)(esi + off); };
    e->sub_2FE0(I(0x270));
    int32_t avail = I(0x14) - I(0x40) * 2;
    const uint16_t* txt = (const uint16_t*)(esi + 0x5c);
    int32_t len = u16len(txt);
    uint16_t buf[600] = {};
    if (!(esi[0x260] & 1)) u16cpy(buf, txt);
    else for (int32_t k = 0; k < len; k++) buf[k] = '*';
    int32_t wText = e->textWidth(buf, u16len(buf));
    uint16_t us[2] = {'_', 0};
    int32_t wCur = e->textWidth(us, u16len(us));
    int32_t cursor = I(0x25c);
    if (cursor == len) wText += wCur;
    if (wText <= avail) { I(0x50) = len - 1; I(0x4c) = 0; return; }
    if (cursor == len) avail -= wCur;
    if (I(0x50) < cursor) {
        I(0x50) = cursor; I(0x4c) = cursor - 1;
        int32_t s = cursor - 1;
        while (s != 0) {
            s = I(0x4c);
            int32_t cnt = I(0x50) - s + 1, wsum = 0, k = 0;
            const uint16_t* q = buf + s;
            for (uint16_t ch = *q; ch && k < cnt; k++) {
                if (ch != 10) wsum += e->glyph()[3];
                ch = *++q;
            }
            if (avail <= wsum) break;
            s = --I(0x4c);
        }
        I(0x4c)++;
        return;
    }
    int32_t s = I(0x4c);
    if (cursor <= s + 1) {
        int32_t c = cursor - 1;
        c &= (c < 0) - 1;
        I(0x4c) = c; I(0x50) = c + 1;
        // loop test uses the current end index
        while (I(0x50) < len && e->textWidth(buf + I(0x4c), I(0x50) - I(0x4c) + 1) < avail) I(0x50)++;
        I(0x50)--;
        return;
    }
    if (avail <= e->textWidth(buf + s, u16len(buf + s))) {
        I(0x50) = s + 1;
        while (I(0x50) < len && e->textWidth(buf + I(0x4c), I(0x50) - I(0x4c) + 1) < avail) I(0x50)++;
        I(0x50)--;
        return;
    }
    I(0x50) = len - 1; I(0x4c) = len - 2;
    s = len - 2;
    while (s != 0) {
        s = I(0x4c);
        if (avail <= e->textWidth(buf + s, u16len(buf + s))) break;
        s = --I(0x4c);
    }
    I(0x4c)++;
}
