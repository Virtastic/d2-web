// src/core/d2client/d2client_batch_6eb70.cpp -- chat-input history / line helpers, D2Client.dll 1.13c
// (alpha lane: implementation + marker, no spec, no vector). Base 0x6FAB0000.
// Globals live in D2cChatState (RVA beside each field). Unrecovered callees (ordinals, Win32) go through
// the D2cHost seam: D2cCall.rva = the ordinal number, or 0xF000000n for Win32 (1 GetTickCount,
// 2 GetSystemDefaultLangID, 3 ImmIsIME(GetKeyboardLayout(0))); registers/stack words as retail passes them.
// NOT DONE: 0x6EC30 (s3509) -- its decompile reads register/stack slots (unaff_EBX, unaff_retaddr, stack0x..)
// the decompiler could not resolve; a faithful port needs the disassembly, not a guess.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"

extern "C" int32_t d2_client_3d6c(int32_t c);   // _toupper

struct D2cChatState {
    int32_t  cur;          // 0xC3F5C DAT_6fbc3f5c, history cursor
    int32_t  langMode;     // 0xC3F60
    uint8_t  count;        // 0xCC024
    int32_t  lineLen;      // 0xCC028
    int32_t  head, tail;   // 0x927D0 / 0x927D4
    int32_t  blink;        // 0xA9EA4
    uint32_t blinkMs;      // 0xA9EA0
    uint32_t lastTick;     // 0xD3450
    int32_t  rightEdge;    // 0x8BC48
    int32_t  baseY;        // 0x8BC4C
    uint16_t hist[8][0x101];  // 0xCEE88, stride 0x202
    uint16_t line[0x101];     // 0xCEC80
    uint16_t name[1];         // 0xCEC40 (NUL-terminated wide string; sized by caller's buffer)
    const uint16_t* msg2;     // 0xCFE98
    int32_t* list;            // 0xCC034 (list object: +8 n, +0x10 first, +0x14 span, +0x18 offsets)
};
static_assert(sizeof(((D2cChatState*)0)->hist[0]) == 0x202, "history stride");

static int32_t wlen(const uint16_t* s) { int32_t n = 0; while (s[n]) ++n; return n; }

// 1.13c D2Client+0x6EB70 (62 B, caller 0x6FCE0): in=eax, out=ebx. host(571,eax=in) returns the end
// offset into `in` (0 = none); copy [0,end) to out + NUL, else host(501) handles it.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6eb70(const char* in, char* out, D2cHost h) {
    if (*in != '*') {
        D2cCall c = {}; c.rva = 571; c.eax = (uint32_t)(uintptr_t)in;
        uint32_t end = h(&c);
        if (end != 0) { memcpy(out, in, end); out[end] = 0; return; }
    }
    D2cCall c = {}; c.rva = 501; h(&c);
}

// 1.13c D2Client+0x6EC00 (41 B, caller 0x707F0): retail re-reads w[len] (the terminator) every pass.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_6ec00(const uint16_t* w) {
    int32_t len = wlen(w);
    uint32_t hi = ((uint32_t)len >> 8) & 0xFFFFFF;
    for (int32_t i = 0; i < len; ++i)
        if (w[len] > 0xFF) return hi << 8;
    return (hi << 8) | 1;
}

// 1.13c D2Client+0x6EDF0 (75 B, fastcall ecx=delta, caller 0x6F200)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6edf0(D2cChatState* s, int32_t d) {
    int32_t i = s->cur + d;
    if (i < 8) { if (i < 0) i = 7; } else i = 0;
    if (s->count < 7 && (int32_t)s->count < 8 - i) i = s->cur;
    s->cur = i;
}

// 1.13c D2Client+0x6EE40 (116 B, edx = 0x202-byte entry)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6ee40(D2cChatState* s, const uint16_t* e) {
    if (s->count == 8) {
        int32_t t = s->tail - 1;
        s->head = s->tail; s->tail = t;
        if (t < 0) s->tail = 7;
    } else {
        if (s->count != 0 && (--s->head) < 0) s->head = 7;
        s->count++;
    }
    s->cur = s->head;
    memcpy(s->hist[s->head], e, 0x202);
}

// 1.13c D2Client+0x6EEC0 (51 B, caller 0x6F200)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6eec0(D2cChatState* s) {
    memcpy(s->line, s->hist[s->cur], 0x202);
    s->lineLen = wlen(s->hist[s->cur]);
}

// 1.13c D2Client+0x6EF20 (129 B, thiscall; callers 0x70140, 0x6F3E0): caret blink.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6ef20(D2cChatState* s, uint32_t self, uint32_t p, D2cHost h) {
    if (s->blink == 1) {
        D2cCall a = {}; a.rva = 10003; a.ecx = self; a.s[0] = p; a.s[1] = 0; h(&a);
        D2cCall b = {}; b.rva = 10150; h(&b);
    }
    D2cCall t = {}; t.rva = 0xF0000001; uint32_t now = h(&t);
    if (s->blinkMs < now - s->lastTick) {
        s->lastTick = now;
        if (s->blink != 0) { s->blinkMs = 100; s->blink = 0; return; }
        s->blinkMs = 500; s->blink = 1;
    }
}

// 1.13c D2Client+0x6EFB0 (139 B, callers 0x70700, 0x70500): a=eax, b=stack. Case-insensitive
// "b begins with a" compare over strlen(a); -1 if b shorter or b<a, +1 if b>a, 0 equal.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_6efb0(const char* a, const char* b) {
    size_t n = strlen(a);
    if (strlen(b) < n) return -1;
    for (; n; --n, ++a, ++b) {
        int32_t x = d2_client_3d6c((int32_t)*b), y = d2_client_3d6c((int32_t)*a);
        if (x != y) return (x - y >= 0) ? 1 : -1;
    }
    return 0;
}

// 1.13c D2Client+0x6F060 (80 B, caller 0x6F5B0): IME-active test.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_6f060(const D2cChatState* s, D2cHost h) {
    if (s->langMode == 9 || s->langMode == 8) {
        D2cCall c = {}; c.rva = 0xF0000002; uint32_t id = h(&c) & 0xFFFF;
        if (s->langMode == 9) return id == 0x404 || id == 0xC04;
        return id == 0x804 || id == 0x1004;
    }
    D2cCall c = {}; c.rva = 0xF0000003; return h(&c) != 0;
}

// 1.13c D2Client+0x6F200 (83 B): key handler on event struct p (+8 key, +0xC flags).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6f200(D2cChatState* s, uint32_t* p, D2cHost h) {
    p[6] = 1; p[7] = 0;
    D2cCall c = {}; c.rva = 511; c.eax = (uint32_t)(uintptr_t)p; h(&c);
    if ((p[3] & 0x40000000) == 0) {
        if (p[2] == 0x26) { d2_client_6eec0(s); d2_client_6edf0(s, 1); }
        else if (p[2] == 0x28) { d2_client_6edf0(s, -1); d2_client_6eec0(s); }
    }
}

// 1.13c D2Client+0x6F260 (379 B, caller 0x70140): draw the numbered message list "n: text".
// Ordinal 10083 = line height, 10150 = draw text (args as retail: x/y computed, 0,0).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6f260(D2cChatState* s, D2cHost h) {
    int32_t* L = s->list;
    if (!L) return;
    int32_t first, end;
    if (L[5] == 0) { end = L[2]; first = 0; if (end > 0x13) end = 0x14; }
    else { first = L[4]; end = L[5] + first; if (L[2] < end) end = L[2]; }
    D2cCall hc = {}; hc.rva = 10083; int32_t lh = (int32_t)h(&hc);
    int32_t y = 0, n = 0;
    for (int32_t i = first; i < end; ++i) {
        int32_t off = L[6 + i];
        ++n; y += lh;
        D2cCall a = {}; a.rva = 10150; a.ecx = (uint32_t)(y - 0x170 + s->baseY); a.s[0] = 0; a.s[1] = 0;
        a.s[2] = (uint32_t)n; h(&a);                    // "%d: " label (s[2] = number)
        D2cCall b = {}; b.rva = 10150; b.ecx = a.ecx;
        b.eax = (uint32_t)(uintptr_t)((const char*)L + off);   // MultiByteToWideChar of the entry
        h(&b);
        if (y > 0x118) return;
    }
}

// 1.13c D2Client+0x6F3E0 (191 B, caller 0x70140): x=eax, y=stack; two chat segments, wrap right.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6f3e0(D2cChatState* s, const uint16_t* name, int32_t x, int32_t y, D2cHost h) {
    int32_t w1 = 0;
    D2cCall wc = {}; wc.rva = 10028;
    if (wlen(name) != 0) {
        w1 = (int32_t)h(&wc);
        if (s->rightEdge - 0x109 < w1 - 0x83 + x) { x = 0x83; y += 0xF; }
        D2cCall d = {}; d.rva = 10150; d.ecx = (uint32_t)y; d.s[0] = 4; h(&d);
    }
    int32_t w2 = (int32_t)h(&wc);
    if (wlen(s->msg2) != 0) {
        if (s->rightEdge - 0x109 < x + w2 - 0x83 + w1) y += 0xF;
        D2cCall d = {}; d.rva = 10150; d.ecx = (uint32_t)y; d.s[0] = 4; h(&d);
    }
    d2_client_6ef20(s, (uint32_t)y, 4, h);
}
