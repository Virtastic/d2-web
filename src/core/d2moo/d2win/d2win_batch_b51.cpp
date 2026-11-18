// src/core/d2win/d2win_batch_b51.cpp -- 1.13c D2Win.dll small functions (alpha batch lane, no spec).
// Transcribed from the cached decompile. Retail globals/callees arrive through an env struct; offsets
// in comments are the retail 32-bit layout (pointers widen).
#include <cstdint>
#include <emscripten.h>

struct D2WinCtl {                  // control header, common prefix
    uint32_t type;                 // +0
    uint32_t pad[7];
    uint32_t cb20;                 // +0x20 (index 8): the retail 4-byte code address
    uint32_t pad2[6];
    uint32_t next;                 // +0x3C: retail 4-byte guest pointer (see d2win_sub_17fc0)
    uint32_t pad3[2];
    uint32_t v48;                  // +0x48 (type 1 only)
};
// Guest pointers are 4-byte words in retail; the env carries the globals as word slots (the same width as the
// host pointers they replace, so the env layout is unchanged).
struct D2WinB51Env {
    void (*fatal)(uint32_t stringId);        // Ordinal_10265 + Ordinal_10024("") then _exit(-1)
    uint32_t* listHead;                      // 0x6F9014A0
    uint32_t* g14b0; uint32_t* g14ac;        // 0x6F9014B0 / 0x6F9014AC
    uint32_t* g14b8; uint32_t* g9cec;        // 0x6F9014B8 / 0x6F9A9CEC
    uint32_t* g14b4;                         // 0x6F9014B4
    void (*freeCtl)(D2WinCtl*);              // 0x17A00
    void (*sub_B50)(uint32_t, int32_t, int32_t); // 0xBB50
    uint32_t* g_ffd30;                       // 0x6F8FFD30
};

// 1.13c D2Win+0x17AF0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17af0(const D2WinB51Env* e) { *e->g_ffd30 = 0; return 1; }

// 1.13c D2Win+0x16470
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_16470(D2WinCtl* c, void* cb6360, const D2WinB51Env* e) {
    if (c->type != 8) e->fatal(0x45);
    // Retail stores one dword at +0x20 (the 4-byte address); a pointer-width store would also zero +0x24..+0x27.
    *(uint32_t*)((uint8_t*)c + 0x20) = (uint32_t)(uintptr_t)cb6360;
}

// 1.13c D2Win+0x136e0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_136e0(D2WinCtl* c, uint32_t v, const D2WinB51Env* e) {
    if (!c) e->fatal(0x445);
    if (c->type != 1) e->fatal(0x448);
    // Store at the retail byte offset: the C struct above widens its pointers, so v48 sits at 0x50 here.
    *(uint32_t*)((uint8_t*)c + 0x48) = v;
}

// 1.13c D2Win+0xBD00
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_bd00(uint32_t p1, int32_t p2, int32_t p3, int32_t p4, const D2WinB51Env* e) {
    int32_t t = p4 ? 5 : 4, i;
    if (p2 < t) i = 0;
    else if (p2 < (p4 ? 10 : 8)) i = 1;
    else i = (p2 >= (p4 ? 15 : 12)) ? 3 : 2;
    if (p3 && i) i += 3;
    e->sub_B50(p1, i, p4);
}

// 1.13c D2Win+0x11E80
// lineCount source: string of UTF-16 code units; fontByte = byte at (font table[curFont].ptr + 10).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_11e80(const uint16_t* s, uint32_t fontByte, int32_t scale, const D2WinB51Env* e) {
    if (!s) e->fatal(0x25e);
    int32_t n = 1;
    for (const uint16_t* p = s; *p; ++p) if (*p == 10) ++n;
    return (int32_t)(fontByte * (uint32_t)scale * (uint32_t)n) / 10;
}

// 1.13c D2Win+0x17fc0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_17fc0(D2WinCtl* c, const D2WinB51Env* e) {
    const uint32_t cw = (uint32_t)(uintptr_t)c;      // the control as a retail guest word
    if (*e->g14b0 == cw) *e->g14b0 = 0;
    if (*e->g14ac == cw) *e->g14ac = 0;
    if (*e->g14b8 == cw) { *e->g14b8 = 0; *e->g9cec = 0; }
    if (*e->g14b4 == cw) *e->g14b4 = 0;
    const uint32_t h = *e->listHead;
    if (!h) return 0;
    if (h == cw) { *e->listHead = c->next; e->freeCtl(c); return 1; }
    D2WinCtl* prev = (D2WinCtl*)(uintptr_t)h;
    uint32_t cur = prev->next;
    if (cur) {
        while (cur != cw) {
            prev = (D2WinCtl*)(uintptr_t)cur; cur = prev->next;
            if (!cur) return 1;
        }
        prev->next = c->next;
        e->freeCtl(c);
    }
    return 1;
}
