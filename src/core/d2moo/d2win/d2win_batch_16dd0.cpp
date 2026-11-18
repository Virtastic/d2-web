// src/core/d2win/d2win_batch_16dd0.cpp -- 1.13c D2Win.dll 0x16DD0 (ordinal 10076): build a list-box
// control (type 7) from an item table and link it at the tail of the control list (head 0x6F9014A0).
// Transcribed from the cached decompile and checked against the retail bytes. Alpha batch lane: no spec.
// Retail __fastcall: x in ECX, y in EDX; w, h, items, pFontIdx on the stack (ret 0x10).
// items: 16-byte entries {u32 stringId @0, u8 @4, u32 @8, u32 @0xC}, ends at a zero first dword. Offsets in comments are the retail 32-bit layout; pointers widen.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2WinListItem {            // 0x224 bytes in retail
    uint16_t text[256];           // +0x000
    int32_t  width;               // +0x200  text width + 8
    uint32_t v204;                // +0x204  = entry+8
    uint32_t z208, z20c, z210, z214;
    uint8_t  b218;                // +0x218  = byte at entry+0
    uint32_t v21c;                // +0x21C  = entry+4
    D2WinListItem* next;          // +0x220
};
struct D2WinListBox {             // 0x6C bytes in retail
    uint32_t type, z4, flags;     // +0, +4, +8 (7, 0, mask|mask)
    uint32_t x, y, w, h;          // +0xC..+0x18
    void *cb1c, *cb24, *z28, *cb2c, *cb30;
    uint32_t z34, z3c;
    D2WinListBox* next;           // +0x3C (control-list link)
    uint32_t fontIdx, fontH, fontQ, count; // +0x40..+0x50
    int32_t  sel;                 // +0x54 = -1
    uint32_t z5c;
    int32_t  rows;                // +0x58 = h / (fontH + q*2) - 1
    int32_t  maxW;                // +0x60
    D2WinListItem* head;          // +0x64
    D2WinListItem* cur;           // +0x68
};
struct D2WinBatch16DD0Env {
    void* (*alloc)(uint32_t size, const char* file, int32_t line, int32_t zero);  // 0x7678
    uint32_t bitMask0, bitMask8;      // *gdwBitMasks, gdwBitMasks[2]
    void* cb1c; void* cb24; void* cb2c; void* cb30;   // 0x69F0, 0x6600, 0x6590, 0x67C0
    const uint32_t* fontTab;          // 0x6F8FC550
    void (*setFont)(uint32_t);        // 0x12FE0
    const uint16_t* (*getString)(uint16_t); // 0x7780
    void (*widthPrep)();              // 0x11E80
    int32_t (*textWidth)(const uint16_t*);  // 0x11FB0
    D2WinListBox** listHead;          // 0x6F9014A0
    const char* file;                 // 0x6F8FC52C
    void (*fatal)(const uint16_t*);   // 0x76A2 + 0x7684 then _exit(-1)
};

// 1.13c D2Win+0x16DD0
extern "C" EMSCRIPTEN_KEEPALIVE
D2WinListBox* d2win_sub_16dd0(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                              const void* items, const uint32_t* pFontIdx,
                              const D2WinBatch16DD0Env* e) {
    D2WinListBox* b = (D2WinListBox*)e->alloc(0x6C, e->file, 0x2e, 0);
    std::memset(b, 0, sizeof *b);
    b->type = 7;
    b->flags = e->bitMask8 | e->bitMask0;
    b->x = x; b->y = y; b->w = w; b->h = h;
    b->cb1c = e->cb1c; b->cb24 = e->cb24; b->cb2c = e->cb2c; b->cb30 = e->cb30;
    b->fontIdx = *pFontIdx;
    b->fontH = e->fontTab[b->fontIdx];
    b->fontQ = (int32_t)e->fontTab[b->fontIdx] >> 2;
    b->sel = -1;
    b->rows = (int32_t)h / (int32_t)(b->fontH + b->fontQ * 2) - 1;
    e->setFont(*pFontIdx);
    for (const uint32_t* q = (const uint32_t*)items; *q != 0; q += 4) {
        D2WinListItem* it = (D2WinListItem*)e->alloc(0x224, e->file, 0x56, 0);
        const uint16_t* str = e->getString((uint16_t)q[0]);
        int32_t n = 0; while (str[n]) ++n;                 // Unicode::strlen
        if (n >= 0x100) e->fatal(str);                     // does not return
        std::memcpy(it->text, str, (n + 1) * 2);           // Unicode::strcpy
        int32_t tw = e->textWidth(str);
        e->widthPrep();
        tw += 8;
        if (b->maxW < tw) b->maxW = tw;
        it->width = tw;
        it->v204 = q[3];
        it->z208 = it->z20c = it->z210 = it->z214 = 0;
        it->b218 = (uint8_t)q[1];
        it->v21c = q[2];
        it->next = nullptr;
        if (!b->head) b->head = it;
        else { D2WinListItem* t = b->head; while (t->next) t = t->next; t->next = it; }
        ++b->count;
    }
    e->setFont(1);
    b->cur = b->head;
    D2WinListBox* t = *e->listHead;
    if (!t) *e->listHead = b;
    else { while (t->next) t = t->next; t->next = b; }
    return b;
}
